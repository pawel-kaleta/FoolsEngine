#pragma once

#include "Mesh.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"
#include "FoolsEngine/Assets/AssetAccessors.h"

namespace YAML { class Emitter; class Node; }

namespace fe::Render::Representation
{
	struct ACRenderMeshCore final : public AssetComponent
	{
		AssetID mMeshID;
		AssetID mMaterialID;

		void Init() { mMeshID = NullAssetID; mMaterialID = NullAssetID; }
	};

	class RenderMeshObserver : public AssetInterface
	{
	public:
		const ACRenderMeshCore& GetCore() const { return Get<ACRenderMeshCore>(); }

	protected:
		RenderMeshObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) { }
	};

	class RenderMeshUser : public RenderMeshObserver
	{
	public:
		ACRenderMeshCore& GetCore() const { return Get<ACRenderMeshCore>(); }

	protected:
		RenderMeshUser(ECS_AssetHandle ECS_handle) : RenderMeshObserver(ECS_handle) { }
	};

	class RenderMesh final : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::RenderMesh; }
		static constexpr const char* GetMetaFileExtension() { return ".ferm"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACRenderMeshCore>(assetID).Init(); }

		using Observer = RenderMeshObserver;
		using User = RenderMeshUser;
		using Core = ACRenderMeshCore;
	};
}