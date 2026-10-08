#pragma once

#include "FoolsEngine/Foundation/Memory/Splice.h"
#include "FoolsEngine/Foundation/Memory/Arena.h"
#include "FoolsEngine/Foundation/Memory/Pool.h"


#include "FoolsEngine/Assets/Asset.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Renderer/1 - GAPI/Stream.h"
#include "FoolsEngine/Renderer/2 - Representation/Material.h"

namespace fe::Render::Representation
{
	class MaterialAllocator
	{
		struct Region
		{
			Region* mPrevious;
			Region* mNext;
			AssetID mMaterialID;
			U32 mSize;
			U32 mOffset;
		};

		GAPI::GID mBuffer;
		Region* mFirstRegion;
		Region* mLastRegion;
		U32 mFreeOffset;
		U32 mBufferSize;
		U32 mCopyBudget;

		DynamicPool<Region, 64> mRegions;

		void Init()
		{
			mBuffer = GAPI::GID();
			mFirstRegion = nullptr;
			mFreeOffset = 0;
			mBufferSize = 0;
			mCopyBudget = 0;
			mRegions.Init();
		}

		void Create(UInt bufferSize, UInt copyBudget)
		{
			mFreeOffset = 0;
			mFirstRegion = nullptr;
			mBufferSize = bufferSize;
			mCopyBudget = copyBudget;
			mBuffer = GAPI::CreateBuffer();
			GAPI::AllocateCommitBuffer(mBuffer, bufferSize);
		}

		UInt AvailableCapacity() { return mBufferSize - mFreeOffset; }

		void FreeMesh(AssetUser<Material>& materialUser)
		{
			auto region_component = materialUser.Get_GPU<GAPI::Platform::OpenGL>();

			Region* region = (Region*)region_component->mRegion;

			FE_CORE_ASSERT(region, "AAAAA!");
			FE_CORE_ASSERT(materialUser.GetID() == region->mMaterialID, "AAAAA!");

			materialUser.Remove_GPU<GAPI::Platform::OpenGL>();
			region->mMaterialID = NullAssetID;

			Region* prev_region = region->mPrevious;
			Region* next_region = region->mNext;

			// coalessing left
			if (prev_region)
			{
				if (prev_region->mMaterialID == NullAssetID)
				{
					prev_region->mSize += region->mSize;
					prev_region->mNext = region->mNext;
					if (next_region)
						next_region->mPrevious = prev_region;

					mRegions.Remove(region);

					region = prev_region;
					prev_region = prev_region->mPrevious;
				}
			}
			// coalessing right
			if (next_region)
			{
				if (next_region->mMaterialID == NullAssetID)
				{
					region->mSize += next_region->mSize;
					region->mNext = next_region->mNext;
					next_region->mNext->mPrevious = region;

					mRegions.Remove(next_region);

					next_region = region->mNext;
				}
			}
			else
			{
				prev_region->mNext = nullptr;
				mFreeOffset = prev_region->mOffset + prev_region->mSize;
				mRegions.Remove(region);
				mLastRegion = prev_region;
			}
		}

		void Compact()
		{
			if (!mFirstRegion)
				return;

			Region* potencial_empty_region = mFirstRegion;
			UInt copy_budget = 0;

			while (potencial_empty_region->mMaterialID != NullAssetID)
			{
				potencial_empty_region = potencial_empty_region->mNext;
				if (!potencial_empty_region)
					return;
			}

			Region* empty_region = potencial_empty_region;

			if (!empty_region->mNext)
			{
				FE_CORE_ASSERT(false, "Shouldnt be possible"); // FreeMesh should not leave empty regions at the end
				return;
			}

			while (empty_region->mNext->mMaterialID != NullAssetID)
			{
				if (copy_budget >= mCopyBudget)
					break;

				auto& l_region = *empty_region;
				auto& r_region = *(empty_region->mNext);

				{
					AssetUser<Material> mesh_user(r_region.mMaterialID);

					if (l_region.mSize >= r_region.mSize)
					{
						copy_budget += r_region.mSize;
						GAPI::CopyRegionCmd(mBuffer, r_region.mOffset, r_region.mSize, mBuffer, l_region.mOffset);
					}
					else
					{
						copy_budget += r_region.mSize * 2;

						FE_CORE_ASSERT(Context::GPU::ScrachBufferSize >= r_region.mSize, "Region bigger then scrachBuffer");

						GAPI::CopyRegionCmd(mBuffer, r_region.mOffset, r_region.mSize, Context::GPU::ScrachBuffer, 0);
						GAPI::CopyRegionCmd(Context::GPU::ScrachBuffer, r_region.mOffset, r_region.mSize, mBuffer, l_region.mOffset);
					}

					l_region.mMaterialID = r_region.mMaterialID;
					r_region.mMaterialID = NullAssetID;
					std::swap(l_region.mSize, r_region.mSize);
					r_region.mOffset = l_region.mSize + l_region.mOffset;

					auto region_component = mesh_user.Get_GPU<GAPI::Platform::OpenGL>();
					region_component->mRegion = &l_region;
					region_component->mBufferOffset = l_region.mOffset;
				}

				// coalessing rigth
				while (r_region.mNext)
				{
					if (r_region.mNext->mMaterialID != NullAssetID)
						break;

					auto region = (Region*)&r_region;
					auto next_region = r_region.mNext;

					region->mSize += next_region->mSize;
					region->mNext = next_region->mNext;
					next_region->mNext->mPrevious = region;

					mRegions.Remove(next_region);
				}

				if (!r_region.mNext)
				{
					l_region.mNext = nullptr;
					mFreeOffset = r_region.mOffset;
					mRegions.Remove(&r_region);
					mLastRegion = &l_region;
					break;
				}

				empty_region = &r_region;
			}
		}

		bool AllocateMesh(AssetUser<Material>& materialUser)
		{
			auto& core_component = materialUser.GetCore();

			UInt alloc_size = core_component.DataSizeGPU();
			UInt alloc_offset = GAPI::GetOffsetAlignmentFor(alloc_size);

			if (mFreeOffset + alloc_size > mBufferSize)
				return false;

			auto region = mRegions.Emplace();
			region->mMaterialID = materialUser.GetID();
			region->mOffset = mFreeOffset;
			region->mSize = alloc_size;
			region->mNext = nullptr;
			region->mPrevious = mLastRegion;

			mLastRegion = region;

			auto& region_component = materialUser.Emplace_GPU<GAPI::Platform::OpenGL>();
			region_component.mBuffer = mBuffer;
			region_component.mBufferOffset = mFreeOffset;
			region_component.mRegion = region;

			mFreeOffset += alloc_size;

			return true;
		}
	};
}