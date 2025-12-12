#pragma once

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\1 - Description\Texture.h"
#include "FoolsEngine\Renderer\2 - Resource\Texture.h"
#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetInterface.h"

namespace YAML { class Emitter; class Node; }

namespace fe
{
	struct GAPIType;

	struct ACTexture2DCore final : public AssetComponent
	{
		void* Data;
		Description::Texture::Usage Usage;
		uint32_t SpecificationID;
		uint32_t Width;
		uint32_t Height;

		void Init()
		{
			Width = 0;
			Height = 0;
			Usage = Description::Texture::Usage::None;
			SpecificationID = -1;
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
		const ACTexture2DCore& GetCoreComponent() const { return Get<ACTexture2DCore>(); }

		template <GAPIType::ValueType GAPI>
		const auto& GetResourceComponent() const
		{
			if constexpr (GAPI == GAPIType::OpenGL) return Get<ACTexture2DResource_OpenGL>();
		}

		void SaveMetadata(YAML::Emitter& emitter);

	protected:
		Texture2DObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class Texture2DUser : public Texture2DObserver
	{
	public:
		ACTexture2DCore& GetCoreComponent() const { return Get<ACTexture2DCore>(); }
		
		template <GAPIType::ValueType GAPI>
		auto& GetResourceComponent() const
		{
			if constexpr (GAPI == GAPIType::OpenGL) return Get<ACTexture2DResource_OpenGL>();
		}

		template <GAPIType::ValueType GAPI>
		auto& CreateResourceComponent() const
		{
			if constexpr (GAPI == GAPIType::OpenGL) return Emplace<ACTexture2DResource_OpenGL>();
		}

		 bool LoadMetadata();

		bool SendDataToGPU(GAPIType GAPI) const;
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
		static AssetID LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath);

		using Observer = Texture2DObserver;
		using User = Texture2DUser;
		typedef ACTexture2DCore Core;
	};
}