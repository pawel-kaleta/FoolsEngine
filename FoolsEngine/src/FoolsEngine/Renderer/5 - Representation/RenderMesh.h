#pragma once

#include "Mesh.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"
#include "FoolsEngine/Assets/AssetAccessors.h"

namespace YAML { class Emitter; class Node; }

namespace fe
{
	struct ACRenderMeshCore final : public AssetComponent
	{
		AssetID MeshID;
		AssetID MaterialID;

		size_t DataSizeGPU;
		size_t DataSizeCPU;

		void Init() { MeshID = NullAssetID; MaterialID = NullAssetID; DataSizeGPU = 0; DataSizeCPU = 0;	}
	};

	class RenderMeshObserver : public AssetInterface
	{
	public:
		const ACRenderMeshCore& GetCore() const { return Get<ACRenderMeshCore>(); }

		size_t GetCPUDataSize() const { const auto& core = Get<ACRenderMeshCore>(); return core.DataSizeCPU; }
		size_t GetGPUDataSize() const { const auto& core = Get<ACRenderMeshCore>(); return core.DataSizeGPU; }
	protected:
		RenderMeshObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) { }
	};

	class RenderMeshUser : public RenderMeshObserver
	{
	public:
		ACRenderMeshCore& GetCore() const { return Get<ACRenderMeshCore>(); }

		void Release() const;

		bool SendDataToGPU(GAPIType GAPI) const;
		void UnloadFromCPU() const;
	protected:
		RenderMeshUser(ECS_AssetHandle ECS_handle) : RenderMeshObserver(ECS_handle) { }
	};

	class RenderMesh final : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::RenderMesh; }
		static constexpr const char* GetMetaFileExtension() { return ".ferm"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACRenderMeshCore>(assetID).Init(); }
		static void SaveMetadata(YAML::Emitter& emitter, AssetID assetID);
		static bool LoadMetadata(AssetID assetID);
		static AssetID LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath);

		using Observer = RenderMeshObserver;
		using User = RenderMeshUser;
		using Core = ACRenderMeshCore;
	};
}