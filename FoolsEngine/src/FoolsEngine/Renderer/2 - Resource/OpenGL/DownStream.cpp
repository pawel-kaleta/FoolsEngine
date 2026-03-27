#include "FE_pch.h"

#include "FoolsEngine/Renderer/2 - Resource/DownStream.h"
#include "FoolsEngine/Foundation/Memory/Pile.h"
#include "FoolsEngine/Foundation/Memory/Allocators/Global.h"

namespace fe::Resource
{
	void DownStream_OpenGL::Create(UInt capacity)
	{
		auto gpa_alloc = StableAllocs::GeneralPurpose;
		FrontFences.InitXarrAlloc(&Alloc, gpa_alloc);
		BackFences.InitXarrAlloc(&Alloc, gpa_alloc);
		Regions.InitXarrAlloc(&Alloc, gpa_alloc);
		RegionFences.InitXarrAlloc(&Alloc, gpa_alloc);

		CurrentOffset = 0;

		glCreateBuffers(1, &OpenGLBuffer);

		GLbitfield map_flags	= GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
		GLbitfield create_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT;

		UInt aligned_capacity = ((capacity + (Alignment - 1)) & ~(Alignment - 1));

		glNamedBufferStorage(OpenGLBuffer, aligned_capacity, nullptr, create_flags);
		CPUMemoryBegin = (Byte*)glMapNamedBufferRange(OpenGLBuffer, 0, aligned_capacity, map_flags);
		Capacity = aligned_capacity;
	}

	void DownStream_OpenGL::Destroy()
	{
		FE_LOG_CORE_ERROR("DownStream_OpenGL::Destroy not implemented yet -> memory leak");
		glDeleteBuffers(1, &OpenGLBuffer);
	}

	void DownStream_OpenGL::BeginRegion()
	{
		auto& new_fence = * BackFences.PushBack();
		new_fence.OpenGLFence = nullptr;
		new_fence.Location = CurrentOffset;
		RegionFences.Append(&new_fence);

		auto& region = *Regions.PushBack();
		region.Stream = this;
		region.OpenGLBuffer = OpenGLBuffer;
		region.Offset = CurrentOffset;
		region.Size = 0;
	}

	void DownStream_OpenGL::PushData(void* data, UInt size)
	{
		auto& region = Regions[Regions.Count];

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
				void* region_begin = CPUMemoryBegin + region.Offset;
				std::memmove(CPUMemoryBegin, region_begin, region.Size);

				region.Offset = 0;
				CurrentOffset = region.Size;
			}
		}

		if (data) std::memcpy(CPUMemoryBegin + CurrentOffset, data, size);
		CurrentOffset += size;
		region.Size += size;
	}

	StreamRegion* DownStream_OpenGL::EndRegion()
	{
		auto& region = Regions[Regions.Count];
		UInt aligned_size = ((region.Size + (Alignment - 1)) & ~(Alignment - 1));

		UInt padding = aligned_size - region.Size;
		if (padding) PushData(nullptr, padding);

		glFlushMappedNamedBufferRange(OpenGLBuffer, region.Offset, aligned_size);

		return &region;
	}

	StreamRegion* DownStream_OpenGL::ReserveUncommitedRegion(UInt size)
	{
		auto& new_fence = *BackFences.PushBack();
		new_fence.OpenGLFence = nullptr;
		new_fence.Location = CurrentOffset;
		RegionFences.Append(&new_fence);

		auto& region = *Regions.PushBack();
		region.Stream = this;
		region.OpenGLBuffer = OpenGLBuffer;
		region.Offset = CurrentOffset;
		region.Size = 0;

		UInt aligned_size = ((size + (Alignment - 1)) & ~(Alignment - 1));

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
		UInt region_index;
		for (UInt i = 0; i < Regions.Chunks.Count; i++)
		{
			UInt chunk_capacity = UInt(1) << i;
			UInt chunk_left_bound = (UInt)Regions.Chunks[i];
			UInt chunk_right_bound = chunk_left_bound + chunk_capacity * sizeof(StreamRegion_OpenGL*);
			if (chunk_left_bound <= (UInt)region && (UInt)region < chunk_right_bound)
			{
				UInt index_in_chunk = ((UInt)region - (UInt)Regions.Chunks[i]) / sizeof(StreamRegion_OpenGL*);

				U64 capacity_of_smaller_chunks = -1;
				// shift by 64 as 1 op is UB and sets result to -1
				capacity_of_smaller_chunks = capacity_of_smaller_chunks >> (63 - i);
				capacity_of_smaller_chunks = capacity_of_smaller_chunks >> 1;

				region_index = capacity_of_smaller_chunks + index_in_chunk;
			}
		}
		Fence* fence_ptr = RegionFences[region_index];
		if (fence_ptr)
			fence_ptr->OpenGLFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}

	void DownStream_OpenGL::EndFrame()
	{
		Pile p;
#ifdef FE_INTERNAL_BUILD
		auto region_fences = RegionFences.GetCopyContiguous(&p);

		for (UInt i = 0; i<region_fences.Count; i++)
		{
			auto& element = region_fences[i];
			if (element)// if buffer resized, we don't currently verify retirement of regions that no longer need fences
				if (!element->OpenGLFence)
				{
					FE_LOG_CORE_ERROR("Unretired DownStream Region at the end of the frame!");
				}
		}

#endif // FE_INTERNAL_BUILD
		RegionFences.Count = 0;
		Regions.Count = 0;

		glDeleteBuffers(PastBuffersToDestroy.Count, PastBuffersToDestroy.begin());
		PastBuffersToDestroy.Release();
	}

	bool DownStream_OpenGL::CheckFences(UInt pushSize)
	{
		UInt new_offset = CurrentOffset + pushSize;

		while (NextFenceIndex < FrontFences.Count)
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

	void DownStream_OpenGL::MakeNewBuffer(UInt pushSize)
	{
		auto& region = Regions[Regions.Count];
		Byte* region_begin = CPUMemoryBegin + region.Offset;

		PastBuffersToDestroy.Append(OpenGLBuffer);

		// we cannot reset RegionFences.Count to 0, because this is a proxy from region to fence
		// and they are syncronised region-proxy
		// and there are regions in flight still from PastBuffersToDestroy
		// we nullify ptrs instead
		UInt chunk_capacity = 1;
		for (UInt i = 0; i < RegionFences.Chunks.Count; i++)
		{
			std::memset(RegionFences.Chunks[0], (int)nullptr, chunk_capacity * sizeof(UInt));
			chunk_capacity = chunk_capacity << 1;
		}

		for (UInt i = 0; i < FrontFences.Count; i++)
			glDeleteSync(FrontFences[i].OpenGLFence); //glDeleteSync will silently ignore a sync value of zero (nullptr)
		for (UInt i = 0; i < BackFences.Count; i++)
			glDeleteSync(BackFences[i].OpenGLFence); //glDeleteSync will silently ignore a sync value of zero (nullptr)
		
		FrontFences.Count = 0;
		BackFences.Count = 0;
		NextFenceIndex = 0;

		glCreateBuffers(1, &OpenGLBuffer);

		GLbitfield map_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
		GLbitfield create_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT;

		UInt new_capacity = Capacity + pushSize;
		new_capacity *= 1.5f;
		UInt aligned_capacity = ((new_capacity + (Alignment - 1)) & ~(Alignment - 1));

		glNamedBufferStorage(OpenGLBuffer, aligned_capacity, nullptr, create_flags);
		CPUMemoryBegin = (Byte*)glMapNamedBufferRange(OpenGLBuffer, 0, aligned_capacity, map_flags);
		Capacity = aligned_capacity;

		std::memcpy(CPUMemoryBegin, region_begin, region.Size);

		CurrentOffset = region.Size;
		region.Offset = 0;

		auto& fence = *BackFences.PushBack();
		fence.OpenGLFence = nullptr;
		fence.Location = 0;
		RegionFences[RegionFences.Count] = &fence;
	}
}