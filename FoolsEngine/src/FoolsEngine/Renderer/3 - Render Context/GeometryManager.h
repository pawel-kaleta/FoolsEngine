#pragma once

#include "FoolsEngine/Foundation/Memory/Splice.h"
#include "FoolsEngine/Foundation/Memory/Arena.h"
#include "FoolsEngine/Foundation/Memory/Pool.h"


#include "FoolsEngine/Assets/Asset.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Renderer/2 - Representation/Mesh.h"

namespace fe::Render::Representation
{
	class GeometryManager
	{
		struct Region
		{
			Region* mPrevious;
			Region* mNext;
			AssetID mMeshID;
			U32 mSize;
			U32 mOffset;
		};

		GAPI::GID mBuffer;
		GAPI::GID mScrachBuffer;
		Region* mFirstRegion;
		U32 mScrachBufferSize;
		U32 mFreeOffset;
		U32 mBufferSize;
		U32 mCopyBudget;

		DynamicPool<Region, 64> mRegions;


		void Init()
		{
			mBuffer = GAPI::GID();
			mScrachBuffer = GAPI::GID();
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
			FE_CORE_ASSERT(meshUser.GetID() == region->mMeshID, "AAAAA!");

			region->mMeshID = NullAssetID;

			meshUser.RemoveRegionGPU<GAPI::Platform::OpenGL>();

			Region* prev_region = region->mPrevious;
			Region* next_region = region->mNext; // what if null ??!!

			// coalessing left
			if (prev_region->mMeshID == NullAssetID)
			{
				prev_region->mSize += region->mSize;
				prev_region->mNext = region->mNext;
				next_region->mPrevious = prev_region;

				mRegions.Remove(region);

				region = prev_region;
				prev_region = prev_region->mPrevious;
			}
			// coalessing right
			if (next_region->mMeshID == NullAssetID)
			{
				region->mSize += next_region->mSize;
				region->mNext = next_region->mNext;
				next_region->mNext->mPrevious = region;

				mRegions.Remove(next_region);

				next_region = region->mNext;
			}
		}

		void Compact()
		{
			if (!mFirstRegion)
				return;

			Region* potencial_empty_region = mFirstRegion;
			UInt copy_budget = 0;

			while (potencial_empty_region->mMeshID != NullAssetID)
			{
				potencial_empty_region = potencial_empty_region->mNext;
				if (!potencial_empty_region)
					return;
			}

			Region* empty_region = potencial_empty_region;
			while (empty_region->mNext->mMeshID != NullAssetID) // what if mNext null ??!!
			{
				if (copy_budget >= mCopyBudget)
					break;

				auto& l_region = *empty_region;
				auto& r_region = *(empty_region->mNext); // what if mNext null ??!!

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
				// naming change vs main loop?
				while (r_region.mNext)
				{
					if (r_region.mNext->mMeshID != NullAssetID)
						break;

					auto region = (Region*) & r_region;
					auto next_region = r_region.mNext;

					region->mSize += next_region->mSize;
					region->mNext = next_region->mNext;
					next_region->mNext->mPrevious = region;

					mRegions.Remove(next_region);

					next_region = region->mNext;
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