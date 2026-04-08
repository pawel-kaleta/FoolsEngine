#include "FE_pch.h"

#include "FoolsEngine/Renderer/2 - Resource/RDownStream.h"
#include "FoolsEngine/Foundation/Memory/Pile.h"
#include "FoolsEngine/Foundation/Memory/Allocators/StableAllocs.h"
#include "FoolsEngine/Foundation/Utils/Context.h"

namespace fe::Resource
{
	void RDownStream<GAPIType::OpenGL>::Create(UInt capacity)
	{
		auto default_alloc = Context::Allocators::Default;
		auto auxilary_alloc = Context::Allocators::Auxiliary;

		Alloc.Init(default_alloc);
		FrontFences.InitXarrAlloc(&Alloc, auxilary_alloc);
		BackFences.InitXarrAlloc(&Alloc, auxilary_alloc);
		Regions.InitXarrAlloc(&Alloc, auxilary_alloc);
		RegionFences.InitXarrAlloc(&Alloc, auxilary_alloc);
		PastBuffersToDestroy.Init(auxilary_alloc);

		CurrentOffset = 0;

		glCreateBuffers(1, &OpenGLBuffer);

		GLbitfield map_flags	= GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
		GLbitfield create_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT;

		UInt aligned_capacity = ((capacity + (Alignment - 1)) & ~(Alignment - 1));

		glNamedBufferStorage(OpenGLBuffer, aligned_capacity, nullptr, create_flags);
		CPUMemoryBegin = (Byte*)glMapNamedBufferRange(OpenGLBuffer, 0, aligned_capacity, map_flags);
		Capacity = aligned_capacity;
	}

	void RDownStream<GAPIType::OpenGL>::Destroy()
	{
		FE_LOG_CORE_ERROR("DownStream_OpenGL::Destroy not implemented yet -> memory leak");
		glDeleteBuffers(1, &OpenGLBuffer);
	}

	void RDownStream<GAPIType::OpenGL>::BeginRegion()
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

	void RDownStream<GAPIType::OpenGL>::PushData(Splice<Byte> data)
	{
		auto& region = Regions[Regions.Count];
		UInt push_size = data.Count;

		if (CheckFences(push_size))
			MakeNewBuffer(push_size);

		if (CurrentOffset + push_size > Capacity)
		{
			std::swap(BackFences, FrontFences);
			NextFenceIndex = 0;
			CurrentOffset = 0;
			
			if (CheckFences(push_size + region.Size))
				MakeNewBuffer(push_size);
			else
			{
				void* region_begin = CPUMemoryBegin + region.Offset;
				std::memmove(CPUMemoryBegin, region_begin, region.Size);

				region.Offset = 0;
				CurrentOffset = region.Size;
			}
		}

		if (data.Elements) std::memcpy(CPUMemoryBegin + CurrentOffset, data.Elements, push_size);
		CurrentOffset += push_size;
		region.Size += push_size;
	}

	StreamRegion* RDownStream<GAPIType::OpenGL>::EndRegion()
	{
		auto& region = Regions[Regions.Count];
		UInt aligned_size = ((region.Size + (Alignment - 1)) & ~(Alignment - 1));

		Splice<Byte> padding;
		padding.Count = aligned_size - region.Size;
		if (padding.Count) PushData(padding);

		glFlushMappedNamedBufferRange(OpenGLBuffer, region.Offset, aligned_size);

		return &region;
	}

	StreamRegion* RDownStream<GAPIType::OpenGL>::ReserveUncommitedRegion(UInt size)
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

		Splice<Byte> push_placeholder;
		push_placeholder.Count = ((size + (Alignment - 1)) & ~(Alignment - 1));
		PushData(push_placeholder);

		return &region;
	}

	void RDownStream<GAPIType::OpenGL>::CommitRegion(StreamRegion* region)
	{
		RStreamRegion_OpenGL* region_opengl = (RStreamRegion_OpenGL*)region;
		glFlushMappedNamedBufferRange(OpenGLBuffer, region_opengl->Offset, region_opengl->Size);
	}

	void RDownStream<GAPIType::OpenGL>::RetireRegion(StreamRegion* region)
	{
		UInt region_index;
		for (UInt i = 0; i < Regions.Chunks.Count; i++)
		{
			UInt chunk_capacity = UInt(1) << i;
			UInt chunk_left_bound = (UInt)Regions.Chunks[i];
			UInt chunk_right_bound = chunk_left_bound + chunk_capacity * sizeof(RStreamRegion_OpenGL*);
			if (chunk_left_bound <= (UInt)region && (UInt)region < chunk_right_bound)
			{
				UInt index_in_chunk = ((UInt)region - (UInt)Regions.Chunks[i]) / sizeof(RStreamRegion_OpenGL*);

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

	void RDownStream<GAPIType::OpenGL>::EndFrame()
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

	bool RDownStream<GAPIType::OpenGL>::CheckFences(UInt pushSize)
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
					glDeleteSync(next_fence.OpenGLFence);
					next_fence.OpenGLFence = nullptr;
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

	void RDownStream<GAPIType::OpenGL>::MakeNewBuffer(UInt pushSize)
	{
		auto& region = Regions[Regions.Count];
		Byte* region_begin = CPUMemoryBegin + region.Offset;

		* PastBuffersToDestroy.PushBack() = OpenGLBuffer;

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