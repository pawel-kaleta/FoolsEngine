#pragma once

#include "FoolsEngine/Foundation/Memory/Splice.h"
#include "FoolsEngine/Foundation/Memory/Pool.h"


#include "FoolsEngine/Assets/Asset.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Renderer/1 - GAPI/Stream.h"

#include "FoolsEngine/Renderer/2 - Representation/Mesh.h"

namespace fe::Render::Representation
{
	class MeshAllocator
	{
		struct ACRegion final : AssetComponent 
		{
			AssetID mPrevious;
			AssetID mNext;
			U32 mSize;
			U32 mOffset;
			U32 mAlignment;
		};

		GAPI::GID mBuffer;
		AssetID mFirstMesh;
		AssetID mLastMesh;
		U32 mFreeOffset;
		U32 mBufferSize;
		U32 mCopyBudget;

		void Init()
		{
			mBuffer = GAPI::GID();
			mFirstMesh = NullAssetID;
			mLastMesh = NullAssetID;
			mFreeOffset = 0;
			mBufferSize = 0;
			mCopyBudget = 0;
		}

		void Create(UInt bufferSize, UInt copyBudget)
		{
			mFreeOffset = 0;
			mFirstMesh = NullAssetID;
			mLastMesh = NullAssetID;
			mBufferSize = bufferSize;
			mCopyBudget = copyBudget;
			mBuffer = GAPI::CreateBuffer();
			GAPI::AllocateCommitBuffer(mBuffer, bufferSize);
		}

		UInt AvailableCapacity() { return mBufferSize - AlignOffsetTo(mFreeOffset, GAPI::GetOffsetAlignmentFor<Vertex>()); }

		void FreeMesh(AssetUser<Mesh>& meshUser)
		{
			auto& region = meshUser.Get<ACRegion>();

			AssetID prev_mesh = region.mPrevious;
			AssetID next_mesh = region.mNext;	

			if (bool(prev_mesh) && bool(next_mesh))
			{
				AssetUser<Mesh>(prev_mesh).Get<ACRegion>().mNext = next_mesh;
				AssetUser<Mesh>(next_mesh).Get<ACRegion>().mPrevious = prev_mesh;
			}
			else if (!bool(prev_mesh) && bool(next_mesh))
			{
				mFirstMesh = next_mesh;
				AssetUser<Mesh>(next_mesh).Get<ACRegion>().mPrevious = prev_mesh;
			}
			else if (bool(prev_mesh) && !bool(next_mesh))
			{
				AssetUser<Mesh> user(prev_mesh);
				auto& prev_region = user.Get<ACRegion>();
				prev_region.mNext = next_mesh;
				mLastMesh = prev_mesh;
				mFreeOffset = prev_region.mOffset + prev_region.mSize;
			}
			else if (!bool(prev_mesh) && !bool(next_mesh))
			{
				mFirstMesh = NullAssetID;
				mLastMesh = NullAssetID;
				mFreeOffset = 0;
			}

			meshUser.Erase<ACRegion>();
		}

		void Compact()
		{
			if (!mFirstMesh)
				return;

			UInt copy_budget = 0;
			UInt current_offest = 0;
			AssetID mesh = mFirstMesh;

			while (mesh && (copy_budget < mCopyBudget))
			{
				AssetUser<Mesh> user(mesh);
				auto& region = user.Get<ACRegion>();
				UInt available_offset = AlignOffsetTo(current_offest, region.mAlignment);

				if (region.mOffset > available_offset)
				{
					if (region.mAlignment + region.mSize > region.mOffset)
					{
						copy_budget += region.mSize * 2;

						GAPI::CopyRegionCmd(mBuffer, region.mOffset, region.mSize, Context::GPU::ScrachBuffer, 0);
						GAPI::CopyRegionCmd(Context::GPU::ScrachBuffer, 0, region.mSize, mBuffer, available_offset);
					}
					else
					{
						copy_budget += region.mSize;
						GAPI::CopyRegionCmd(mBuffer, region.mOffset, region.mSize, mBuffer, available_offset);
					}
					region.mOffset = available_offset;
				}

				mesh = region.mNext;
				current_offest = (UInt)region.mOffset + region.mSize;
			}
		}

		bool AllocateMesh(AssetUser<Mesh>& meshUser)
		{
			auto& core_component = meshUser.GetCore();

			UInt alloc_size = core_component.DataSize();
			UInt alloc_alignment = GAPI::GetOffsetAlignmentFor<Vertex>();
			UInt alloc_offset = AlignOffsetTo(mFreeOffset, alloc_alignment);

			if (alloc_offset + alloc_size > mBufferSize)
				return false;

			auto& region = meshUser.Emplace<ACRegion>();
			region.mOffset = alloc_offset;
			region.mSize = alloc_size;
			region.mAlignment = alloc_alignment;
			region.mNext = NullAssetID;
			region.mPrevious = mLastMesh;

			mLastMesh = meshUser.GetID();
			mFreeOffset = alloc_offset + alloc_size;

			return true;
		}
	};
}