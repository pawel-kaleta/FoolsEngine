#pragma once

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/1 - Description/Texture.h"
#include "FoolsEngine/Renderer/2 - Resource/Texture.h"

namespace YAML { class Emitter; class Node; }

namespace fe
{
	struct GAPIType;

	struct ACTexture2DCore final : public AssetComponent
	{
		void* Data;
		Description::Texture::Specification Specification;

		void Init()
		{
			Specification.Width = 0;
			Specification.Height = 0;
			Specification.Usage = Description::Texture::Usage::None;
			Specification.ArchetypeID = -1;
			Specification.BorderColor = { 0, 0, 0 };
			Data = nullptr;
		}
	};

	struct ACTexture2DResource_OpenGL final : public AssetComponent
	{
		Resource::Texture_OpenGL Texture;
	};

	class Texture2DObserver : public AssetInterface
	{
	public:
		const ACTexture2DCore& GetCore() const { return Get<ACTexture2DCore>(); }

		void SaveMetadata(YAML::Emitter& emitter);

	protected:
		Texture2DObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class Texture2DUser : public Texture2DObserver
	{
	public:
		ACTexture2DCore& GetCore() const { return Get<ACTexture2DCore>(); }

		bool LoadMetadata();

		void UnloadFromCPU() const;
		void Release() const;

	protected:
		Texture2DUser(ECS_AssetHandle ECS_handle) : Texture2DObserver(ECS_handle) {}
	};

	class Texture2D : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Texture2D; }
		static constexpr const char* GetMetaFileExtension() { return ".fetex2d"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACTexture2DCore>(assetID).Init(); }
		static void SaveMetadata(YAML::Emitter& emitter, AssetID assetID) {}
		static bool LoadMetadata(AssetID assetID) { return false; }
		static AssetID LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath);

		using Observer = Texture2DObserver;
		using User = Texture2DUser;
		typedef ACTexture2DCore Core;
	};
}