#pragma once

#include "FoolsEngine/Foundation/Memory/XarAlloc.h"
#include "FoolsEngine/Foundation/Memory/DynArrAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MonotonicAlloc.h"
#include "FoolsEngine/Foundation/Memory/Allocators/MallocAlloc.h"

#include <glad/glad.h>

namespace fe::GAPI::DownStream
{
	struct Stream;

	struct Region
	{
		Stream* Stream;
		U32 Offset;
		U32 Size;
	};

	struct Stream
	{
		GLuint OpenGLBuffer = 0;
		U32 Capacity = 0;
		U32 CurrentOffset = 0;
		Byte* DMABegin = nullptr;
		UInt NextFenceIndex = 0;

		struct Fence
		{
			GLsync OpenGLFence;
			UInt Location;
		};

		SpliceArena<Fence> FrontFences;
		SpliceArena<Fence> BackFences;

		SpliceArena<Region> Regions;
		SpliceArena<Fence*> RegionFences;

		void Make(U32 size, U32 maxRegionCount)
		{
			CurrentOffset = 0;

			glCreateBuffers(1, &OpenGLBuffer);

			GLbitfield create_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
			GLbitfield map_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;

			glNamedBufferStorage(OpenGLBuffer, size, nullptr, create_flags);
			DMABegin = (Byte*)glMapNamedBufferRange(OpenGLBuffer, 0, size, map_flags);
			Capacity = size;
		}
		void Release()
		{
			FE_CORE_ASSERT(false, "Not implemented");
		}

		void BeginRegion(U32 alignment)
		{
			CurrentOffset = ((CurrentOffset + (alignment - 1)) & ~(alignment - 1));

			auto& new_fence = *BackFences.PushBack();
			new_fence.OpenGLFence = nullptr;
			new_fence.Location = CurrentOffset;
			RegionFences.Append(&new_fence);

			auto& region = *Regions.PushBack();
			region.Stream = this;
			region.Offset = CurrentOffset;
			region.Size = 0;
		};
		void PushData(Splice<Byte> data)
		{
			auto& region = Regions[Regions.Count];
			UInt push_size = data.Count;

			CheckFences(push_size);

			if (CurrentOffset + push_size > Capacity)
			{
				FrontFences.Count = 0;
				std::swap(BackFences, FrontFences);
				NextFenceIndex = 0;
				CurrentOffset = 0;

				CheckFences(push_size + region.Size);

				void* region_old_begin = DMABegin + region.Offset;
				FE_CORE_ASSERT(false, "Stream read is illegal"); // mem move
				std::memmove(DMABegin, region_old_begin, region.Size);

				region.Offset = 0;
				CurrentOffset = region.Size;

			}

			if (data.Elements) std::memcpy(DMABegin + CurrentOffset, data.Elements, push_size);
			CurrentOffset += push_size;
			region.Size += push_size;
		};
		Region* EndRegion() {};

		Region* ReserveRegion(U32 size, U32 alignment) {};
		void CommitRegion(Region* region) {};

		void RetireRegion(Region* region) {};

		void CheckFences(UInt pushSize)
		{
			UInt new_offset = CurrentOffset + pushSize;

			while (NextFenceIndex < FrontFences.Count)
			{
				auto& next_fence = FrontFences[NextFenceIndex];

				if (next_fence.OpenGLFence) // false means region not even retired yet 
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
					auto result = glClientWaitSync(FrontFences[NextFenceIndex].OpenGLFence, GL_SYNC_FLUSH_COMMANDS_BIT, 10000);

					switch (result)
					{
					case GL_ALREADY_SIGNALED:
						continue;

					case GL_WAIT_FAILED:
						FE_CORE_ASSERT(false, "Down stream sync error!");
					case GL_TIMEOUT_EXPIRED:
					case GL_CONDITION_SATISFIED:
						FE_CORE_ASSERT(false, "Down stream stall on fence!");
					}
				}
			}
		}
	};




	struct RStreamRegion {};

	struct RDownStream
	{
		virtual void Create(UInt capacity) = 0;
		virtual void Destroy() = 0;

		virtual void BeginRegion() = 0;
		virtual void PushData(Splice<Byte> data) = 0;
		virtual RStreamRegion* EndRegion() = 0;

		virtual RStreamRegion* ReserveUncommitedRegion(Splice<Byte> & CPURegion) = 0;
		virtual void CommitRegion(RStreamRegion* region) = 0;

		virtual void RetireRegion(RStreamRegion* region) = 0;

		virtual void EndFrame() = 0;
	};

	struct RDownStream_OpenGL;

	struct RStreamRegion_OpenGL final : public RStreamRegion
	{
		RDownStream_OpenGL* Stream;
		UInt Offset;
		GLuint OpenGLBuffer;
		U32 Size;
	};

	struct RDownStream_OpenGL final : public RDownStream
	{
		static inline constexpr UInt Alignment = 64;

		struct Fence
		{
			GLsync OpenGLFence;
			UInt Location;
		};
		
		MonotonicAlloc Alloc;

		XarrAlloc<Fence, MonotonicAlloc, Allocator> FrontFences;
		XarrAlloc<Fence, MonotonicAlloc, Allocator> BackFences;
		UInt NextFenceIndex;
		
		XarrAlloc<RStreamRegion_OpenGL, MonotonicAlloc, Allocator> Regions;
		XarrAlloc<Fence*, MonotonicAlloc, Allocator> RegionFences;

		UInt CurrentOffset;
		Byte* CPUMemoryBegin;
		UInt Capacity;
		GLuint OpenGLBuffer;

		// When we outgrow a buffer (hit a fence), we cannot orphan/recreate it,
		// because there may stil be unsceduled draw calls involving it
		// instead we postpone destruction to the end of the frame
		// when we are guaranteed, that all draw call are allready issued
		DynArrAlloc<GLuint, Allocator> PastBuffersToDestroy;

		virtual void Create(UInt capacity = 64) override;
		virtual void Destroy() override;

		virtual void BeginRegion() override;
		virtual void PushData(Splice<Byte> data) override;
		virtual RStreamRegion* EndRegion() override;

		virtual RStreamRegion* ReserveUncommitedRegion(Splice<Byte>& CPURegion) override;
		virtual void CommitRegion(RStreamRegion* region) override;

		virtual void RetireRegion(RStreamRegion* region) override;

		virtual void EndFrame() override;

	private:
		bool CheckFences(UInt pushSize);
		void MakeNewBuffer(UInt pushSize);
	};
}