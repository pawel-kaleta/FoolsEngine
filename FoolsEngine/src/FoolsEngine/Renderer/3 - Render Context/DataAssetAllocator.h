#pragma once

#include "FoolsEngine/Foundation/Memory/Splice.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetTypes.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Renderer/1 - GAPI/Stream.h"

namespace fe::Render
{
	class DataAssetAllocator
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
		AssetID mFirstAsset;
		AssetID mLastAsset;
		U32 mFreeOffset;
		U32 mBufferSize;
		U32 mCopyBudget;

		void Init()
		{
			mBuffer = GAPI::GID();
			mFirstAsset = NullAssetID;
			mLastAsset = NullAssetID;
			mFreeOffset = 0;
			mBufferSize = 0;
			mCopyBudget = 0;
		}

		void Create(UInt bufferSize, UInt copyBudget)
		{
			mFreeOffset = 0;
			mFirstAsset = NullAssetID;
			mLastAsset = NullAssetID;
			mBufferSize = bufferSize;
			mCopyBudget = copyBudget;
			mBuffer = GAPI::CreateBuffer();
			GAPI::AllocateCommitBuffer(mBuffer, bufferSize);
		}

		UInt AvailableCapacity() { return mBufferSize - mFreeOffset; }

		void FreeAsset(AssetUser& assetUser)
		{
			auto& region = assetUser.Get<ACRegion>();

			AssetID prev = region.mPrevious;
			AssetID next = region.mNext;

			if (bool(prev) && bool(next))
			{
				AssetUser(prev).Get<ACRegion>().mNext = next;
				AssetUser(next).Get<ACRegion>().mPrevious = prev;
			}
			else if (!bool(prev) && bool(next))
			{
				mFirstAsset = next;
				AssetUser(next).Get<ACRegion>().mPrevious = prev;
			}
			else if (bool(prev) && !bool(next))
			{
				AssetUser user(prev);
				auto& prev_region = user.Get<ACRegion>();
				prev_region.mNext = next;
				mLastAsset = prev;
				mFreeOffset = prev_region.mOffset + prev_region.mSize;
			}
			else if (!bool(prev) && !bool(next))
			{
				mFirstAsset = NullAssetID;
				mLastAsset = NullAssetID;
				mFreeOffset = 0;
			}

			assetUser.Erase<ACRegion>();
		}

		void Compact()
		{
			if (!mFirstAsset)
				return;

			UInt copy_budget = 0;
			UInt current_offest = 0;
			AssetID current_asset = mFirstAsset;

			while (current_asset && (copy_budget < mCopyBudget))
			{
				AssetUser user(current_asset);
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

				current_asset = region.mNext;
				current_offest = (UInt)region.mOffset + region.mSize;
			}
		}

		bool AllocateAsset(AssetUser& assetUser, UInt size, UInt alignment)
		{
			UInt alloc_offset = AlignOffsetTo(mFreeOffset, alignment);

			if (alloc_offset + size > mBufferSize)
				return false;

			auto& region = assetUser.Emplace<ACRegion>();
			region.mOffset = alloc_offset;
			region.mSize = size;
			region.mAlignment = alignment;
			region.mNext = NullAssetID;
			region.mPrevious = mLastAsset;

			mLastAsset = assetUser.GetID();
			mFreeOffset = alloc_offset + size;

			return true;
		}
	};
}