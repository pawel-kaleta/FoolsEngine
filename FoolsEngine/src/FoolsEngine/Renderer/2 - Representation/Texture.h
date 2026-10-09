#pragma once

#include "FoolsEngine/Foundation/Memory/Arena.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"

namespace YAML { class Emitter; class Node; }

namespace fe
{
	struct ACTexture2D_Core final : public AssetComponent
	{
		Render::GAPI::Descriptors::TextureSpec mSpecification;

		void Init()
		{
			mSpecification.Init();
		}

		UInt GetSourceSize() const
		{
			using namespace Render::GAPI::Descriptors;
			UInt texel_size = 0;

			switch (mSpecification.mFormat.Value)
			{
			case TextureFormat::None:
				FE_CORE_ASSERT(false, "Not specified texture data format");
				texel_size = 0;
				break;
			case TextureFormat::R_8:				texel_size = 1; break;
			case TextureFormat::RG_8:				texel_size = 2; break;
			case TextureFormat::RGB_8:				texel_size = 3; break;
			case TextureFormat::RGBA_8:				texel_size = 4; break;
			case TextureFormat::R_UINT_32:			texel_size = 4; break;
			case TextureFormat::DEPTH24STENCIL8:	texel_size = 4; break;
			default:
				FE_CORE_ASSERT(false, "Uknown texture data format");
				texel_size = 0;
			}

			UInt texel_count = (UInt)mSpecification.mDimentions.x * mSpecification.mDimentions.y * mSpecification.mDimentions.z;

			return texel_size * texel_count;
		}
	};

	template <Render::GAPI::Platform::ValueType tPlatform>
	struct ACTexture2D_GPU final : public AssetComponent
	{
		Render::GAPI::GID mTextureGID;
		DynamicArena<Render::GAPI::GID> mTextureViewsGIDs;

		void Init()
		{
			mTextureGID = Render::GAPI::GID();
			mTextureViewsGIDs.Init();
		}
	};

	class Texture2D : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Texture2D; }
		static constexpr const char* GetMetaFileExtension() { return ".fetex2d"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACTexture2D_Core>(assetID).Init(); }
		
		static AssetID LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath);

		using Core = ACTexture2D_Core;
	};
}