#pragma once

#include "FoolsEngine\Renderer\1 - Description\Texture.h"
#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetInterface.h"

namespace YAML { class Emitter; class Node; }

namespace fe
{
	struct GAPIType;

	struct ACTexture2DCore final : public AssetComponent
	{
		Description::Texture::Specification Specification;
		uint32_t Width;
		uint32_t Height;
		Description::Texture::Usage Usage;
		void* Data;

		void Init()
		{
			Width = 0;
			Height = 0;
			Usage = Description::Texture::Usage::None;
			Data = nullptr;
		}
	};

	class Texture2DObserver : public AssetInterface
	{
	public:
		const ACTexture2DCore& GetCoreComponent() const { return Get<ACTexture2DCore>(); }

		uint32_t GetRendererID(GAPIType GAPI) const;

	protected:
		Texture2DObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class Texture2DUser : public Texture2DObserver
	{
	public:
		ACTexture2DCore& GetCoreComponent() const { return Get<ACTexture2DCore>(); }

		void Release() const;

		bool SendDataToGPU(GAPIType GAPI) const;
		void Bind(GAPIType GAPI, RenderTextureSlotID slotID = 0) const;
		void UnloadFromCPU() const;

		template <typename tnGAPITexture2D>
		tnGAPITexture2D& CreateGAPITexture2D(const ACTexture2DCore& core, const void* data) { return Emplace<tnGAPITexture2D>(core.Specification, data, core.Width, core.Height); }

		void CreateGAPITexture2D(GAPIType GAPI) const;

		void CreateGAPITexture2D(GAPIType GAPI, const ACTexture2DCore& core, const void* data) const;

		template <typename tnGAPITexture2D>
		tnGAPITexture2D& CreateGAPITexture2D() const
		{
			auto& core = GetCoreComponent();
			auto& spec = core.Specification;
			void* data = core.Data;
			return Emplace<tnGAPITexture2D>(spec, data, core.Width, core.Height);
		}

	protected:
		Texture2DUser(ECS_AssetHandle ECS_handle) : Texture2DObserver(ECS_handle) {}
	};

	class Texture2D : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Texture2D; }
		static constexpr const char* GetMetaFileExtension() { return ".fetex2d"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACTexture2DCore>(assetID).Init(); }
		static void SaveMetadata(YAML::Emitter& emitter, AssetID assetID);
		static bool LoadMetadata(AssetID assetID);
		static AssetID LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath);

		using Observer = Texture2DObserver;
		using User = Texture2DUser;
		typedef ACTexture2DCore Core;
	};
}