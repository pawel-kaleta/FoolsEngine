#include "FE_pch.h"

#include "FoolsEngine\Renderer\2 - Resource\DownStream.h"

namespace fe::Resource
{
	void DownStream_OpenGL::Create(size_t capacity)
	{
		CurrentOffset = 0;

		glCreateBuffers(1, &OpenGLBuffer);

		GLbitfield map_flags	= GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
		GLbitfield create_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT;

		size_t alignment = 64;
		size_t aligned_capacity = ((capacity + (alignment - 1)) & ~(alignment - 1));

		glNamedBufferStorage(OpenGLBuffer, aligned_capacity, nullptr, create_flags);
		CPUMemoryBegin = glMapNamedBufferRange(OpenGLBuffer, 0, aligned_capacity, map_flags);
		Capacity = aligned_capacity;
	}

	void DownStream_OpenGL::Destroy()
	{
		FE_LOG_CORE_ERROR("DownStream_OpenGL::Destroy not implemented yet -> memory leak");
		//glDeleteBuffers(1, &Buffer);
	}

	void DownStream_OpenGL::BeginRegion()
	{
		CurrentRegionOffset = CurrentOffset;
		BackFences.push({nullptr, CurrentOffset});
		RegionFences.push_back(&BackFences.back());
	}

	void DownStream_OpenGL::PushData(void* data, size_t size)
	{
		CheckFences(CurrentOffset + size);

		if (CurrentOffset + size > Capacity)
		{
			CheckFences(CurrentOffset - CurrentRegionOffset + size);

			if (CurrentOffset + size > Capacity)
			{
				std::swap(BackFences, FrontFences);

				void* region_begin = (uint8_t*)CPUMemoryBegin + CurrentRegionOffset;
				size_t region_size = CurrentOffset - CurrentRegionOffset;
				std::memmove(CPUMemoryBegin, region_begin, region_size);

				CurrentRegionOffset = 0;
				CurrentOffset = region_size;
			}
		}

		if (data) std::memcpy((uint8_t*)CPUMemoryBegin + CurrentOffset, data, size);
		CurrentOffset += size;
	}

	StreamRegion* DownStream_OpenGL::EndRegion()
	{
		size_t size = CurrentOffset - CurrentRegionOffset;
		size_t alignment = 64;
		size_t aligned_size = ((size + (alignment - 1)) & ~(alignment - 1));

		size_t padding = aligned_size - size;
		if (padding) PushData(nullptr, padding);

		glFlushMappedNamedBufferRange(OpenGLBuffer, CurrentRegionOffset, aligned_size);

		auto& region = Regions.emplace_back();
		region.Stream = this;
		region.OpenGLBuffer = OpenGLBuffer;
		region.Offset = CurrentRegionOffset;
		region.Size = aligned_size;

		return &region;
	}

	void DownStream_OpenGL::RetireRegion(StreamRegion* region)
	{
		StreamRegion_OpenGL* region_opengl = (StreamRegion_OpenGL*)region;
		size_t region_index = region_opengl - Regions.data();
		Fence* fence_ptr = RegionFences[region_index];
		if (fence_ptr)
			fence_ptr->OpenGLFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}

	void DownStream_OpenGL::EndFrame()
	{
#ifdef FE_INTERNAL_BUILD
		for (auto& region : RegionFences)
		{
			if (!region->OpenGLFence)
			{
				FE_LOG_CORE_ERROR("Unretired DownStream Region at the end of the frame!");
			}
		}
#endif // FE_INTERNAL_BUILD
	}

	void DownStream_OpenGL::CheckFences(size_t pushSize)
	{
		size_t new_offset = CurrentOffset + pushSize;

		while (FrontFences.size())
		{
			GLsync fence = FrontFences.front().OpenGLFence;
			size_t fence_location = FrontFences.front().Location;

			if (fence)
			{
				GLint sync_status;
				glGetSynciv(fence, GL_SYNC_STATUS, 1, nullptr, &sync_status);

				if (sync_status == GL_SIGNALED)
				{
					FrontFences.pop();
					continue;
				}
			}

			if (new_offset > fence_location)
			{
				return MakeNewBuffer(pushSize);
				break;
			}
		}
	}

	void DownStream_OpenGL::MakeNewBuffer(size_t pushSize)
	{
		void* region_begin = (uint8_t*)CPUMemoryBegin + CurrentRegionOffset;
		size_t region_size = CurrentOffset - CurrentRegionOffset;


		PastBuffersToDestroy.push_back(OpenGLBuffer);

		std::memset(RegionFences.data(), 0, RegionFences.size() * sizeof(void*));

		for (auto& fence : FrontFences._Get_container())
			glDeleteSync(fence.OpenGLFence); //glDeleteSync will silently ignore a sync value of zero (nullptr)
		for (auto& fence : BackFences._Get_container())
			glDeleteSync(fence.OpenGLFence); //glDeleteSync will silently ignore a sync value of zero (nullptr)
		
		{
			// we're pushing to one of them at the bottom of this funcion, so we need to clear them her, thus the scope
			std::queue<Fence> emptyA;
			std::queue<Fence> emptyB;
			std::swap(FrontFences, emptyA);
			std::swap(BackFences, emptyB);
		}

		glCreateBuffers(1, &OpenGLBuffer);

		GLbitfield map_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
		GLbitfield create_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT;

		size_t new_capacity = Capacity;
		new_capacity += pushSize;
		new_capacity *= 1.5;
		size_t alignment = 64;
		size_t aligned_capacity = ((new_capacity + (alignment - 1)) & ~(alignment - 1));

		glNamedBufferStorage(OpenGLBuffer, aligned_capacity, nullptr, create_flags);
		CPUMemoryBegin = glMapNamedBufferRange(OpenGLBuffer, 0, aligned_capacity, map_flags);
		Capacity = aligned_capacity;

		std::memcpy(CPUMemoryBegin, region_begin, region_size);

		CurrentRegionOffset = 0;
		CurrentOffset = region_size;

		BackFences.push({ nullptr, 0 });
		RegionFences.back() = &BackFences.back();
	}
}