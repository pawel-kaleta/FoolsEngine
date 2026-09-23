#pragma once

#include "FoolsEngine/Foundation/Memory/Queue.h"
#include "FoolsEngine/Foundation/Memory/Pool.h"
#include "FoolsEngine/Foundation/Utils/BitOperations.h"

#include "FoolsEngine/Renderer/1 - GAPI/GAPI.h"
#include "Buffer.h"

#include <glad/gl.h>

#include <numeric>

namespace fe::GAPI::OpenGL
{
	struct DownStream : Stream
	{
		Byte* CurrentPosition = nullptr;

		Queue<Fence> FrontFences; // to do: make this dynamic size
		Queue<Fence> BackFences; // to do: make this dynamic size
		Pool<Region> Regions; // to do: remove and use registry
		Splice<Fence*> RegionFences; // to do: remove, and use ptr inside region

		void Init()
		{
			CurrentPosition = nullptr;

			Queue<Fence> FrontFences;
			Queue<Fence> BackFences;
			Pool<Region> Regions;
			Splice<Fence*> RegionFences;
		}

		void Create(U32 size)
		{
			FE_CORE_ASSERT(size, "Size is 0.");

			FrontFences.InitAllocate(maxRegionCount);
			BackFences.InitAllocate(maxRegionCount);
			Regions.InitAllocate(maxRegionCount);
			RegionFences = Context::Allocators::Default->Allocate<Fence*>(maxRegionCount);

			CurrentPosition = nullptr;

			glCreateBuffers(1, &OpenGLBuffer);

			GLbitfield create_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
			GLbitfield map_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;

			glNamedBufferStorage(OpenGLBuffer, size, nullptr, create_flags);
			DMABegin = (Byte*)glMapNamedBufferRange(OpenGLBuffer, 0, size, map_flags);
			Capacity = size;
		}

		void ReleaseCmd()
		{
			while (!FrontFences.IsEmpty())
			{
				auto& opengl_fence = FrontFences.First()->OpenGLFence;
				if (opengl_fence)
					glDeleteSync(opengl_fence);
				FrontFences.PopFront();
			}

			while (!BackFences.IsEmpty())
			{
				auto& opengl_fence = FrontFences.First()->OpenGLFence;
				if (opengl_fence)
					glDeleteSync(opengl_fence);
				FrontFences.PopFront();
			}

			glDeleteBuffers(1, &OpenGLBuffer);

			Context::Allocators::Default->Deallocate(RegionFences);
			Regions.Release();
			BackFences.Release();
			FrontFences.Release();

			OpenGLBuffer = 0;
			Capacity = 0;
			DMABegin = nullptr;
			CurrentPosition = nullptr;
		}

		const InternalID CreateRegion(U32 size, U32 alignment = 16)
		{
			auto alligned_offset = (U32)AlignTo((Byte*)(CurrentPosition - DMABegin), alignment);
			Byte* position_candidate = DMABegin + alligned_offset;
			Byte* region_end_candidate = position_candidate + size;

			while (!FrontFences.IsEmpty())
			{
				if (!FrontFences.First()->OpenGLFence) // false means region not even retired yet and opengl fence not placed
					break;
				
				GLint sync_status;

				glGetSynciv(FrontFences.First()->OpenGLFence, GL_SYNC_STATUS, 1, nullptr, &sync_status);

				if (sync_status != GL_SIGNALED)
					break;
				
				glDeleteSync(FrontFences.First()->OpenGLFence);
				FrontFences.First()->OpenGLFence = nullptr;
				FrontFences.PopFront();
			}

			if (!FrontFences.IsEmpty())
			{
				if (FrontFences.First()->Location < position_candidate)
				{
					FE_LOG_CORE_WARN("Not enough space in DownStream");
					return nullptr;
				}
			}
			else if (region_end_candidate > DMABegin + Capacity) //  need to wrap around (ring buffer)
			{
				alligned_offset = (U32)AlignTo((Byte*)DMABegin, alignment);
				position_candidate = DMABegin + alligned_offset;
				region_end_candidate = position_candidate + size;

				if (region_end_candidate > DMABegin + Capacity) // region most likely bigger then whole stream
				{
					FE_LOG_CORE_WARN("Not enough space in DownStream");
					return nullptr;
				}

				while (!BackFences.IsEmpty())
				{
					if (!BackFences.First()->OpenGLFence) // false means region not even retired yet and opengl fence not placed
						break;

					GLint sync_status;
					glGetSynciv(BackFences.First()->OpenGLFence, GL_SYNC_STATUS, 1, nullptr, &sync_status);

					if (sync_status != GL_SIGNALED)
						break;

					glDeleteSync(BackFences.First()->OpenGLFence);
					BackFences.First()->OpenGLFence = nullptr;
					BackFences.PopFront();
				}

				if (!BackFences.IsEmpty())
				{
					if (BackFences.First()->Location < position_candidate)
					{
						FE_LOG_CORE_WARN("Not enough space in DownStream");
						return nullptr;
					}

					std::swap(BackFences, FrontFences);
				}
			}

			CurrentPosition = region_end_candidate;

			auto new_fence = BackFences.AppendBack();
			new_fence->Location = position_candidate;
			new_fence->OpenGLFence = nullptr;

			Region* region = Regions.Emplace();
			region->Data = position_candidate;
			region->Size = size;
			region->Stream = this;

			auto region_index = region - (Region*)Regions.Buffer.Elements;
			RegionFences[region_index] = new_fence;

			return region;
		};

		void CommitRegion(Region* region)
		{
			FE_CORE_ASSERT(region->Stream == this, "This region is not in this stream");
			glFlushMappedNamedBufferRange(OpenGLBuffer, region->Data - DMABegin, region->Size);
		};

		void RetireRegionCmd(Region* region)
		{
			FE_CORE_ASSERT(region->Stream == this, "This region is not in this stream");
			UInt region_index = region - (Region*)Regions.Buffer.Elements;

			RegionFences[region_index]->OpenGLFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

			region->Size = 0;
			region->Data = nullptr;
			region->Stream = nullptr;
			Regions.Remove(region);
		};
	};
}