#pragma once

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetAccessors.h"


#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Renderer/2 - Representation/Texture.h"

namespace fe::Render
{
	class TextureAllocator
	{
		UInt mTexturesCount;
		UInt mTexturesCountBudget;
		UInt mTexturesSize;
		UInt mTexturesSizeBudget;
		UInt mViewsCount;
		UInt mViewsCountBudget;
		PMAlloc* mAlloc;

		void Init()
		{
			mTexturesCount = 0;
			mTexturesCountBudget = 0;
			mTexturesSize = 0;
			mTexturesSizeBudget = 0;
			mViewsCount = 0;
			mViewsCountBudget = 0;
			mAlloc = nullptr;
		}

		void Create(UInt texturesCountBudget, UInt texturesSizeBudget, UInt viewsCountBudget, PMAlloc* alloc = Context::Allocators::Default)
		{
			mTexturesCount = 0;
			mTexturesCountBudget = texturesCountBudget;
			mTexturesSize = 0;
			mTexturesSizeBudget = texturesSizeBudget;
			mViewsCount = 0;
			mViewsCountBudget = viewsCountBudget;
			mAlloc = alloc;
		}

		bool CreateTexture(AssetUser& textureUser)
		{
			auto& core = textureUser.Get<Texture2D::Core();
			UInt footprint_estimate = core.GetSourceSize();

			if (mTexturesCount + 1 > mTexturesCountBudget)
			{
				return false;
			}
			if (mTexturesSize + footprint_estimate > mTexturesSizeBudget)
			{
				return false;
			}

			mTexturesCount += 1;
			mTexturesSize += footprint_estimate;

			GAPI::GID texture_id = GAPI::CreateTexture(core.mSpecification);
			GAPI::AllocateTexture(texture_id);

			auto& texture_component = textureUser.Emplace<ACTexture2D_GPU<GAPI::Platform::OpenGL>>();
			texture_component.mTextureGID = texture_id;
			
			texture_component.mTextureViewsGIDs.Init(mAlloc);

			return true;
		}

		GAPI::GID CreateTextureView(AssetUser& textureUser, const GAPI::Descriptors::TextureViewSpec& viewSpec)
		{
			auto texture_component = textureUser.GetIfExist<ACTexture2D_GPU<GAPI::Platform::OpenGL>>();

			if (!texture_component)
			{
				FE_CORE_ASSERT(false, "Dont try to create a view for not created texture");
				return GAPI::GID();
			}

			if (mViewsCount + 1 > mViewsCountBudget)
			{
				return GAPI::GID();
			}

			mViewsCount += 1;

			GAPI::GID view_id = GAPI::CreateTextureView(texture_component->mTextureGID, viewSpec);
			GAPI::CommitTextureViewCmd(view_id);
			texture_component->mTextureViewsGIDs.Append(view_id);

			return view_id;
		}

		void FreeTextureView(AssetUser& textureUser, GAPI::GID textureViewGID)
		{
			auto texture_component = textureUser.GetIfExist<ACTexture2D_GPU<GAPI::Platform::OpenGL>>();

			if (!texture_component)
			{
				FE_CORE_ASSERT(false, "Texture does not exist and we are trying to free its view?");
				return;
			}

			FE_CORE_ASSERT(mViewsCount, "TextureViews count is 0, but still we are trying to free some view.");

			bool found = false;
			auto& views = texture_component->mTextureViewsGIDs;
			for (UInt i = 0; i < views.Count; i++)
			{
				if (views[i] != textureViewGID)
					continue;
				
				found = true;
				texture_component->mTextureViewsGIDs.SwapWithBackAndPop(i);
				break;
			}

			FE_CORE_ASSERT(found, "Not found texture view in texture asset");

			GAPI::DestroyTextureViewCmd(textureViewGID);

			--mViewsCount;
		}

		void FreeTexture(AssetUser& textureUser)
		{
			auto& core = textureUser.Get<Texture2D::Core>();
			auto texture_component = textureUser.GetIfExist<ACTexture2D_GPU<GAPI::Platform::OpenGL>>();
			UInt footprint_estimate = core.GetSourceSize();

			FE_CORE_ASSERT(mTexturesCount, "Trying to free texture while there are no textures allocated on gpu");
			FE_CORE_ASSERT(mTexturesSize, "Trying to free texture while there are no textures allocated on gpu");
			FE_CORE_ASSERT(texture_component, "Trying to free texture that is not allocated on gpu");
			FE_CORE_ASSERT(texture_component->mTextureViewsGIDs.Count == 0, "Trying to free texture that still has views to it");

			mTexturesCount -= 1;
			mTexturesSize -= footprint_estimate;

			GAPI::GID& texture_id = texture_component->mTextureGID;
			GAPI::DestroyTextureCmd(texture_id);
			texture_id = GAPI::GID();

			texture_component->mTextureViewsGIDs.Release();
			textureUser.Erase<ACTexture2D_GPU<GAPI::Platform::OpenGL>>();

			return;
		}
	};
}