#pragma once

#include "FoolsEngine/Foundation/Memory/Splice.h"
#include "FoolsEngine/Foundation/Memory/Arena.h"

#include "FoolsEngine/Assets/Asset.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Renderer/2 - Representation/Mesh.h"

namespace fe::Render::Representation
{
	class GeometryManager
	{
		union Region
		{
			struct
			{
				Region* mPrevious;
				Region* mNext;
				AssetID mMeshID;
				U32 mSize;
				U32 mOffset;
			} mRegion;
			struct
			{
				Region* mNext;
			} mFreeListElement;
		};

		GAPI::GID mBuffer;
		GAPI::GID mScrachBuffer;
		Region* mFreeList;
		Region* mFirstRegion;
		U32 mScrachBufferSize;
		U32 mFreeOffset;
		U32 mBufferSize;
		U32 mCopyBudget;

		SpliceArena<Array<Region, 64>> mChunks;

		void Init()
		{
			mBuffer = GAPI::GID();
			mScrachBuffer = GAPI::GID();
			mFreeList = nullptr;
			mFirstRegion = nullptr;
			mScrachBufferSize = 0;
			mFreeOffset = 0;
			mBufferSize = 0;
			mCopyBudget = 0;
		}

		void Create(UInt bufferSize, UInt scrachBufferSize)
		{

		}

		void FreeMesh(AssetUser<Mesh>& meshUser)
		{
			auto region_component = meshUser.GetRegionGPU<GAPI::Platform::OpenGL>();

			Region* region = (Region*) region_component->mRegion;

			FE_CORE_ASSERT(region, "AAAAA!");
			FE_CORE_ASSERT(meshUser.GetID() == region->mRegion.mMeshID, "AAAAA!");

			region->mRegion.mMeshID = NullAssetID;
			meshUser.RemoveRegionGPU<GAPI::Platform::OpenGL>();

			Region* prev_region = region->mRegion.mPrevious;
			Region* next_region = region->mRegion.mNext;

			if (prev_region->mRegion.mMeshID == NullAssetID)
			{
				prev_region->mRegion.mSize += region->mRegion.mSize;
				prev_region->mRegion.mNext = region->mRegion.mNext;
				next_region->mRegion.mPrevious = prev_region;

				region->mFreeListElement.mNext = mFreeList;
				mFreeList = region;

				region = prev_region;
				prev_region = prev_region->mRegion.mPrevious;
			}
			if (next_region->mRegion.mMeshID == NullAssetID)
			{
				region->mRegion.mSize += next_region->mRegion.mSize;
				region->mRegion.mNext = next_region->mRegion.mNext;
				next_region->mRegion.mNext->mRegion.mPrevious = region;

				next_region->mFreeListElement.mNext = mFreeList;
				mFreeList = next_region;

				next_region = region->mRegion.mNext;
			}
		}

		void Compact()
		{
			if (!mFirstRegion)
				return;

			Region* potencial_empty_region = mFirstRegion;
			UInt copy_budget = 0;

			while (potencial_empty_region->mRegion.mMeshID != NullAssetID)
			{
				potencial_empty_region = potencial_empty_region->mRegion.mNext;
			}

			Region* empty_region = potencial_empty_region;
			while (empty_region->mRegion.mNext->mRegion.mMeshID != NullAssetID)
			{
				if (copy_budget >= mCopyBudget)
					break;

				auto& l_region = empty_region->mRegion;
				auto& r_region = empty_region->mRegion.mNext->mRegion;

				if (l_region.mSize >= r_region.mSize)
				{
					copy_budget += r_region.mSize;
					GAPI::CopyRegionCmd(mBuffer, r_region.mOffset, r_region.mSize, mBuffer, l_region.mOffset);
				}
				else
				{
					copy_budget += r_region.mSize * 2;

					GAPI::CopyRegionCmd(mBuffer, r_region.mOffset, r_region.mSize, mScrachBuffer, 0);
					GAPI::CopyRegionCmd(mScrachBuffer, r_region.mOffset, r_region.mSize, mBuffer, l_region.mOffset);
				}

				l_region.mMeshID = r_region.mMeshID;
				r_region.mMeshID = NullAssetID;
				std::swap(l_region.mSize, r_region.mSize);
				r_region.mOffset = l_region.mSize + l_region.mOffset;

				while (r_region.mNext)
				{
					if (r_region.mNext->mRegion.mMeshID != NullAssetID)
						break;

					auto region = (Region*) & r_region;
					auto next_region = r_region.mNext;

					region->mRegion.mSize += next_region->mRegion.mSize;
					region->mRegion.mNext = next_region->mRegion.mNext;
					next_region->mRegion.mNext->mRegion.mPrevious = region;

					next_region->mFreeListElement.mNext = mFreeList;
					mFreeList = next_region;

					next_region = region->mRegion.mNext;
				}

				if (!r_region.mNext)
				{
					l_region.mNext = nullptr;
					((Region*)&r_region)->mFreeListElement.mNext = mFreeList;
					mFreeList = (Region*)&r_region;
					break;
				}
			}
		}

		void AllocateMesh(AssetUser<Mesh>& meshUser)
		{
			if (!mFreeList)
			{

			}
		}
	};
}