#pragma once

#include "FoolsEngine/Foundation/Utils/BitOperations.h"


#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "Buffer.h"

#include <glad/gl.h>

#include <numeric>

namespace fe::Render::GAPI::OpenGL
{
	struct DownStream : Stream
	{
		constexpr static ObjType Type = ObjType::DownStream;

		Byte* mCurrentPosition = nullptr;

		struct Fences
		{
			SpliceArena<Array<Fence, 64>*> mFencesChunks;
			UInt mCount = 0;
			void Init()
			{
				mFencesChunks.Init();
				mCount = 0;
			}
		};

		UInt mNextFrontFenceIndex;
		Fences* mFrontFences;
		Fences* mBackFences;

		void Init(InternalID id)
		{
			mID = id;
			mCurrentPosition = nullptr;
			mNextFrontFenceIndex = 0;

			mOpenGLBuffer = 0;
			mCapacity = 0;
			mDMABegin = nullptr;
		}

		void Create(U32 size)
		{
			FE_CORE_ASSERT(size, "Size is 0.");

			glCreateBuffers(1, &mOpenGLBuffer);

			GLbitfield create_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
			GLbitfield map_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;

			glNamedBufferStorage(mOpenGLBuffer, size, nullptr, create_flags);
			mDMABegin = (Byte*)glMapNamedBufferRange(mOpenGLBuffer, 0, size, map_flags);
			mCapacity = size;

			mFrontFences = Context::Allocators::Default->Allocate<Fences>();
			mBackFences = Context::Allocators::Default->Allocate<Fences>();

			mFrontFences->Init();
			mBackFences->Init();
		}

		void ReleaseCmd()
		{
			while (mNextFrontFenceIndex < mFrontFences->mCount)
			{
				auto& opengl_fence = mFrontFences->mFencesChunks[mNextFrontFenceIndex / 64]->operator[](mNextFrontFenceIndex % 64).OpenGLFence;
				if (opengl_fence)
					glDeleteSync(opengl_fence);
				mNextFrontFenceIndex++;
			}

			UInt mNextBackFenceIndex = 0;
			while (mNextBackFenceIndex < mBackFences->mCount)
			{
				auto& opengl_fence = mBackFences->mFencesChunks[mNextFrontFenceIndex / 64]->operator[](mNextFrontFenceIndex % 64).OpenGLFence;
				if (opengl_fence)
					glDeleteSync(opengl_fence);
				mNextBackFenceIndex++;
			}

			glDeleteBuffers(1, &mOpenGLBuffer);

			for (auto& fencechunk : mFrontFences->mFencesChunks)
			{
				Context::Allocators::Default->Deallocate(fencechunk);
			}
			for (auto& fencechunk : mBackFences->mFencesChunks)
			{
				Context::Allocators::Default->Deallocate(fencechunk);
			}
			mFrontFences->mFencesChunks.Release();
			mBackFences->mFencesChunks.Release();

			mFrontFences = Context::Allocators::Default->Allocate<Fences>();
			mBackFences = Context::Allocators::Default->Allocate<Fences>();

			mOpenGLBuffer = 0;
			mCapacity = 0;
			mDMABegin = nullptr;
			mCurrentPosition = nullptr;
		}

		bool CreateRegion(Region* region, U32 size, U32 alignment = 16)
		{
			auto alligned_offset = (U32)AlignTo((Byte*)(mCurrentPosition - mDMABegin), alignment);
			Byte* position_candidate = mDMABegin + alligned_offset;
			Byte* region_end_candidate = position_candidate + size;

			while (mNextFrontFenceIndex < mFrontFences->mCount)
			{
				auto& OpenGLFence = mFrontFences->mFencesChunks[mNextFrontFenceIndex / 64]->operator[](mNextFrontFenceIndex % 64).OpenGLFence;
				
				if (!OpenGLFence) // false means region not even retired yet and opengl fence not placed
					break;
				
				GLint sync_status;

				glGetSynciv(OpenGLFence, GL_SYNC_STATUS, 1, nullptr, &sync_status);

				if (sync_status != GL_SIGNALED)
					break;
				
				glDeleteSync(OpenGLFence);
				OpenGLFence = nullptr;
				mNextFrontFenceIndex++;
			}

			if (mNextFrontFenceIndex < mFrontFences->mCount)
			{
				auto& fence = mFrontFences->mFencesChunks[mNextFrontFenceIndex / 64]->operator[](mNextFrontFenceIndex % 64);
				if (fence.Location < position_candidate)
				{
					FE_LOG_CORE_WARN("Not enough space in DownStream");
					return false;
				}
			}
			else if (region_end_candidate > mDMABegin + mCapacity) //  need to wrap around (ring buffer)
			{
				alligned_offset = (UInt)AlignTo((Byte*)0, alignment);
				position_candidate = mDMABegin + alligned_offset;
				region_end_candidate = position_candidate + size;

				if (region_end_candidate > mDMABegin + mCapacity) // region most likely bigger then whole stream
				{
					FE_LOG_CORE_WARN("Not enough space in DownStream");
					return false;
				}

				// test how many fences can be passed (dont remove them, we are not commit yet to wraping if we have not enough space)
				UInt next_back_fence_index = 0;
				while (next_back_fence_index < mBackFences->mCount)
				{
					auto& fence = mFrontFences->mFencesChunks[next_back_fence_index / 64]->operator[](next_back_fence_index % 64);
					if (fence.OpenGLFence) // false means region not even retired yet and opengl fence not placed
						break;

					GLint sync_status;
					glGetSynciv(fence.OpenGLFence, GL_SYNC_STATUS, 1, nullptr, &sync_status);

					if (sync_status != GL_SIGNALED)
						break;

					next_back_fence_index++;
				}

				// any fances left after potencial wrap?
				if (next_back_fence_index < mBackFences->mCount)
				{
					auto& fence = mFrontFences->mFencesChunks[next_back_fence_index / 64]->operator[](next_back_fence_index % 64);
					if (fence.Location < position_candidate) // can we fit?
					{
						FE_LOG_CORE_WARN("Not enough space in DownStream");
						return false;
					}
				}

				// we now know we can fit after wrapping, so lets wrap
				mFrontFences->mCount = 0;
				mNextFrontFenceIndex = 0;
				std::swap(mBackFences, mFrontFences);

				// delete all fences we checked before wrapping
				while (mNextFrontFenceIndex < next_back_fence_index)
				{
					auto& OpenGLFence = mFrontFences->mFencesChunks[mNextFrontFenceIndex / 64]->operator[](mNextFrontFenceIndex % 64).OpenGLFence;

					glDeleteSync(OpenGLFence);
					OpenGLFence = nullptr;

					mNextFrontFenceIndex++;
				}
			}

			mCurrentPosition = region_end_candidate;

			auto& chunks_arena = mBackFences->mFencesChunks;
			if (chunks_arena.Count * 64 >= mBackFences->mCount)
			{
				if (chunks_arena.IsFull())
				{
					bool arena_any_capacity = chunks_arena.Count > 1;
					UInt arena_new_capacity = arena_any_capacity ? chunks_arena.Count + (chunks_arena.Count >> 1) : chunks_arena.Count + 1;
					auto new_arena_buffer = Context::Allocators::Auxiliary->Allocate<Array<Fence, 64>*>(arena_new_capacity);

					std::memcpy(new_arena_buffer.Elements, chunks_arena.Buffer.Elements, sizeof(Array<Fence, 64>*) * chunks_arena.Count);
					Context::Allocators::Auxiliary->Deallocate(chunks_arena.Buffer);

					chunks_arena.Buffer = new_arena_buffer;
				}

				auto& chunk_ptr = * mBackFences->mFencesChunks.PushBack();
				chunk_ptr = Context::Allocators::Default->Allocate<Fence, 64>();
			}

			auto& new_fence = mBackFences->mFencesChunks[mBackFences->mCount / 64]->operator[](mBackFences->mCount % 64);
			mBackFences->mCount++;

			new_fence.Location = position_candidate;
			new_fence.OpenGLFence = nullptr;

			region->mData = position_candidate;
			region->mSize = size;
			region->mStream = this;
			region->mFence = &new_fence;

			return true;
		};

		void CommitRegion(Region* region)
		{
			FE_CORE_ASSERT(region->mStream == this, "This region is not in this stream");
			glFlushMappedNamedBufferRange(mOpenGLBuffer, region->mData - mDMABegin, region->mSize);
		};

		void RetireRegionCmd(Region* region)
		{
			FE_CORE_ASSERT(region->mStream == this, "This region is not in this stream");

			region->mFence->OpenGLFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

			region->mSize = 0;
			region->mData = nullptr;
			region->mStream = nullptr;
			region->mFence = nullptr;
		};
	};
}