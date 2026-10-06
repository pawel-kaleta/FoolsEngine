#pragma once

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"

#include <bit>

namespace YAML { class Emitter; class Node; }

namespace fe::Render::Representation
{
	struct GAPIType;

	struct ACTexture2D_Core final : public AssetComponent
	{
		GAPI::Descriptors::TextureSpec mSpecification;

		void Init()
		{
			mSpecification.Init();
		}
	};

	template <GAPI::Platform::ValueType tPlatform>
	struct ACTexture2D_GID final : public AssetComponent
	{
		GAPI::GID mTextureGID;
		GAPI::GID mTextureViewGID;
		GAPI::std140_uvec2 mTextureViewHandle;
	};

	class Texture2DObserver : public AssetInterface
	{
	public:
		const ACTexture2D_Core& GetCore() const { return Get<ACTexture2D_Core>(); }

		void SaveMetadata(YAML::Emitter& emitter);

	protected:
		Texture2DObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class Texture2DUser : public Texture2DObserver
	{
	public:
		ACTexture2D_Core& GetCore() const { return Get<ACTexture2D_Core>(); }

		bool LoadMetadata();

	protected:
		Texture2DUser(ECS_AssetHandle ECS_handle) : Texture2DObserver(ECS_handle) {}
	};

	class Texture2D : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Texture2D; }
		static constexpr const char* GetMetaFileExtension() { return ".fetex2d"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACTexture2D_Core>(assetID).Init(); }
		
		static AssetID LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath);

		using Observer = Texture2DObserver;
		using User = Texture2DUser;
		using Core = ACTexture2D_Core;
	};
}