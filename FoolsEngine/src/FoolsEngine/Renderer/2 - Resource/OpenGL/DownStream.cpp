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
		glDeleteBuffers(1, &OpenGLBuffer);
	}

	void DownStream_OpenGL::BeginRegion()
	{
		BackFences.push_back({nullptr, CurrentOffset});
		RegionFences.push_back(&BackFences.back());

		auto& region = Regions.emplace_back();
		region.Stream = this;
		region.OpenGLBuffer = OpenGLBuffer;
		region.Offset = CurrentOffset;
		region.Size = 0;
	}

	void DownStream_OpenGL::PushData(void* data, size_t size)
	{
		auto& region = Regions.back();

		if (CheckFences(size))
			MakeNewBuffer(size);

		if (CurrentOffset + size > Capacity)
		{
			std::swap(BackFences, FrontFences);
			NextFenceIndex = 0;
			CurrentOffset = 0;
			
			if (CheckFences(size + region.Size))
				MakeNewBuffer(size);
			else
			{
				void* region_begin = (uint8_t*)CPUMemoryBegin + region.Offset;
				std::memmove(CPUMemoryBegin, region_begin, region.Size);

				region.Offset = 0;
				CurrentOffset = region.Size;
			}
		}

		if (data) std::memcpy((uint8_t*)CPUMemoryBegin + CurrentOffset, data, size);
		CurrentOffset += size;
		region.Size += size;
	}

	StreamRegion* DownStream_OpenGL::EndRegion()
	{
		auto& region = Regions.back();
		size_t alignment = 64;
		size_t aligned_size = ((region.Size + (alignment - 1)) & ~(alignment - 1));

		size_t padding = aligned_size - region.Size;
		if (padding) PushData(nullptr, padding);

		glFlushMappedNamedBufferRange(OpenGLBuffer, region.Offset, aligned_size);

		return &region;
	}

	StreamRegion* DownStream_OpenGL::ReserveUncommitedRegion(size_t size)
	{
		BackFences.push_back({ nullptr, CurrentOffset });
		RegionFences.push_back(&BackFences.back());

		auto& region = Regions.emplace_back();
		region.Stream = this;
		region.OpenGLBuffer = OpenGLBuffer;
		region.Offset = CurrentOffset;
		region.Size = 0;

		size_t alignment = 64;
		size_t aligned_size = ((size + (alignment - 1)) & ~(alignment - 1));

		PushData(nullptr, aligned_size);

		return &region;
	}

	void DownStream_OpenGL::CommitRegion(StreamRegion* region)
	{
		StreamRegion_OpenGL* region_opengl = (StreamRegion_OpenGL*)region;
		glFlushMappedNamedBufferRange(OpenGLBuffer, region_opengl->Offset, region_opengl->Size);
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
		for (auto& fence : RegionFences)
		{
			if (fence) // if buffer resized, we don't currently verify retirement of regions that no longer need fences
			if (!fence->OpenGLFence)
			{
				FE_LOG_CORE_ERROR("Unretired DownStream Region at the end of the frame!");
			}
		}
#endif // FE_INTERNAL_BUILD
		RegionFences.clear();
		Regions.clear();

		glDeleteBuffers(PastBuffersToDestroy.size(), PastBuffersToDestroy.data());
		PastBuffersToDestroy.clear();
	}

	bool DownStream_OpenGL::CheckFences(size_t pushSize)
	{
		size_t new_offset = CurrentOffset + pushSize;

		while (NextFenceIndex < FrontFences.size())
		{
			auto& next_fence = FrontFences[NextFenceIndex];

			if (next_fence.OpenGLFence)
			{
				GLint sync_status;
				glGetSynciv(next_fence.OpenGLFence, GL_SYNC_STATUS, 1, nullptr, &sync_status);

				if (sync_status == GL_SIGNALED)
				{
					++NextFenceIndex;
					continue;
				}
			}

			if (new_offset > next_fence.Location)
			{
				return true;
			}
		}

		return false;
	}

	void DownStream_OpenGL::MakeNewBuffer(size_t pushSize)
	{
		auto& region = Regions.back();
		void* region_begin = (uint8_t*)CPUMemoryBegin + region.Offset;

		PastBuffersToDestroy.push_back(OpenGLBuffer);

		std::memset(RegionFences.data(), 0, RegionFences.size() * sizeof(void*));

		for (auto& fence : FrontFences)
			glDeleteSync(fence.OpenGLFence); //glDeleteSync will silently ignore a sync value of zero (nullptr)
		for (auto& fence : BackFences)
			glDeleteSync(fence.OpenGLFence); //glDeleteSync will silently ignore a sync value of zero (nullptr)
		
		FrontFences.clear();
		BackFences.clear();
		NextFenceIndex = 0;

		glCreateBuffers(1, &OpenGLBuffer);

		GLbitfield map_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
		GLbitfield create_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT;

		size_t new_capacity = Capacity + pushSize;
		new_capacity *= 1.5f;
		size_t alignment = 64;
		size_t aligned_capacity = ((new_capacity + (alignment - 1)) & ~(alignment - 1));

		glNamedBufferStorage(OpenGLBuffer, aligned_capacity, nullptr, create_flags);
		CPUMemoryBegin = glMapNamedBufferRange(OpenGLBuffer, 0, aligned_capacity, map_flags);
		Capacity = aligned_capacity;

		std::memcpy(CPUMemoryBegin, region_begin, region.Size);

		CurrentOffset = region.Size;
		region.Offset = 0;

		BackFences.push_back({ nullptr, 0 });
		RegionFences.back() = &BackFences.back();
	}
}