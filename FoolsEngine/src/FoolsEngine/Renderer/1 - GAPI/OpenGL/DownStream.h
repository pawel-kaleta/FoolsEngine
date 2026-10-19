#pragma once

#include "FoolsEngine/Foundation/Utils/BitOperations.h"
#include "FoolsEngine/Foundation/Utils/TypeSetup.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "Buffer.h"

#include <glad/gl.h>

#include <numeric>

namespace fe::Render::GAPI::OpenGL
{
	struct DownStream : public Stream
	{
		constexpr static ObjType Type = ObjType::DownStream;

		DynamicArena<Array<Fence, 64>*> mFencesA;
		DynamicArena<Array<Fence, 64>*> mFencesB;

		Byte* mCurrentPosition;
		
		UInt mNextFrontFenceIndex;
		DynamicArena<Array<Fence, 64>*> * mFrontFences;
		DynamicArena<Array<Fence, 64>*> * mBackFences;
		UInt mFrontFencesCount;
		UInt mBackFencesCount;

		PMAlloc* mMainAlloc;

		FE_TYPE_SETUP(DownStream)

		void Init(InternalID id)
		{
			mID = id;
			mCurrentPosition = nullptr;
			mNextFrontFenceIndex = 0;
			mFrontFencesCount = 0;
			mBackFencesCount = 0;

			mOpenGLBuffer = 0;
			mCapacity = 0;
			mDMABegin = nullptr;

			mFrontFences = mFencesA.P();
			mBackFences = mFencesB.P();

			mMainAlloc = Context::Alloc::Default;
		}

		void Create(U32 size, PMAlloc* mainAlloc = Context::Alloc::Default, PMAlloc* auxAlloc = Context::Alloc::Auxiliary)
		{
			FE_CORE_ASSERT(size, "Size is 0.");

			glCreateBuffers(1, &mOpenGLBuffer);

			GLbitfield create_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
			GLbitfield map_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;

			glNamedBufferStorage(mOpenGLBuffer, size, nullptr, create_flags);
			mDMABegin = (Byte*)glMapNamedBufferRange(mOpenGLBuffer, 0, size, map_flags);
			mCapacity = size;

			mFrontFences = mFencesA.P();
			mBackFences = mFencesB.P();

			mFencesA.Init(auxAlloc);
			mFencesB.Init(auxAlloc);

			mMainAlloc = mainAlloc;
		}

		void ReleaseCmd()
		{
			while (mNextFrontFenceIndex < mFrontFencesCount)
			{
				auto& opengl_fence = mFrontFences->At(mNextFrontFenceIndex / 64)->At(mNextFrontFenceIndex % 64).OpenGLFence;
				if (opengl_fence)
					glDeleteSync(opengl_fence);
				mNextFrontFenceIndex++;
			}

			UInt nextBackFenceIndex = 0;
			while (nextBackFenceIndex < mBackFencesCount)
			{
				auto& opengl_fence = mBackFences->At(nextBackFenceIndex / 64)->At(nextBackFenceIndex % 64).OpenGLFence;
				if (opengl_fence)
					glDeleteSync(opengl_fence);
				nextBackFenceIndex++;
			}

			glDeleteBuffers(1, &mOpenGLBuffer);

			for (auto& fencechunk : mFencesA)
			{
				mMainAlloc->Deallocate(fencechunk);
			}
			for (auto& fencechunk : mFencesB)
			{
				mMainAlloc->Deallocate(fencechunk);
			}
			mFencesA.Release();
			mFencesB.Release();

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

			while (mNextFrontFenceIndex < mFrontFencesCount)
			{
				auto& OpenGLFence = mFrontFences->At(mNextFrontFenceIndex / 64)->At(mNextFrontFenceIndex % 64).OpenGLFence;
				
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

			if (mNextFrontFenceIndex < mFrontFencesCount)
			{
				auto& fence = mFrontFences->At(mNextFrontFenceIndex / 64)->At(mNextFrontFenceIndex % 64);
				if (fence.Location < position_candidate)
				{
					Context::Log::Core.Warn("Not enough space in DownStream"_FE);
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
					Context::Log::Core.Warn("Not enough space in DownStream"_FE);
					return false;
				}

				// test how many fences can be passed (dont remove them, we are not commit yet to wraping if we have not enough space)
				UInt next_back_fence_index = 0;
				while (next_back_fence_index < mBackFencesCount)
				{
					auto& fence = mFrontFences->At(next_back_fence_index / 64)->At(next_back_fence_index % 64);
					if (fence.OpenGLFence) // false means region not even retired yet and opengl fence not placed
						break;

					GLint sync_status;
					glGetSynciv(fence.OpenGLFence, GL_SYNC_STATUS, 1, nullptr, &sync_status);

					if (sync_status != GL_SIGNALED)
						break;

					next_back_fence_index++;
				}

				// any fances left after potencial wrap?
				if (next_back_fence_index < mBackFencesCount)
				{
					auto& fence = mFrontFences->At(next_back_fence_index / 64)->At(next_back_fence_index % 64);
					if (fence.Location < position_candidate) // can we fit?
					{
						Context::Log::Core.Warn("Not enough space in DownStream"_FE);
						return false;
					}
				}

				// we now know we can fit after wrapping, so lets wrap
				mFrontFences->Clear();
				mNextFrontFenceIndex = 0;
				mFrontFencesCount = mBackFencesCount;
				mBackFencesCount = 0;
				std::swap(mBackFences, mFrontFences);

				// delete all fences we checked before wrapping
				while (mNextFrontFenceIndex < next_back_fence_index)
				{
					auto& OpenGLFence = mFrontFences->At(mNextFrontFenceIndex / 64)->At(mNextFrontFenceIndex % 64).OpenGLFence;

					glDeleteSync(OpenGLFence);
					OpenGLFence = nullptr;

					mNextFrontFenceIndex++;
				}
			}

			mCurrentPosition = region_end_candidate;


			if (mBackFences->Count * 64 <= mBackFencesCount)
			{
				mBackFences->Append(mMainAlloc->Allocate<Fence, 64>());
			}

			auto& new_fence = mBackFences->At(mBackFencesCount / 64)->At(mBackFencesCount % 64);
			mBackFencesCount++;

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