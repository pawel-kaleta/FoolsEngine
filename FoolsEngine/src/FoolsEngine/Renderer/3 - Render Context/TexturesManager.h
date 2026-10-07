#pragma once

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetAccessors.h"


#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Renderer/2 - Representation/Texture.h"

namespace fe::Render::Representation
{
	class TexturesManager
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

		bool CreateTexture(AssetUser<Texture2D>& textureUser)
		{
			auto& core = textureUser.GetCore();
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

			auto& texture_component = textureUser.Emplace_GPU<GAPI::Platform::OpenGL>();
			texture_component.mTextureGID = texture_id;
			
			texture_component.mTextureViewsGIDs.Init(mAlloc);

			return true;
		}

		GAPI::GID CreateTextureView(AssetUser<Texture2D>& textureUser, const GAPI::Descriptors::TextureViewSpec& viewSpec)
		{
			auto texture_component = textureUser.Get_GPU<GAPI::Platform::OpenGL>();

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

		void FreeTextureView()
		{

		}

		void FreeTexture()
		{
			
		}
	};
}