#pragma once

#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetInterface.h"

namespace fe
{
	struct ACRenderMeshCore final : public AssetComponent
	{
		AssetID MeshID;
		AssetID MaterialID;

		void Init()
		{
			MeshID = NullAssetID;
			MaterialID = NullAssetID;
		}
	};

	class RenderMeshObserver : public AssetInterface
	{
	public:
		const ACRenderMeshCore& GetCoreComponent() const { return Get<ACRenderMeshCore>(); }

	protected:
		RenderMeshObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) { }
	};

	class RenderMeshUser : public RenderMeshObserver
	{
	public:
		ACRenderMeshCore& GetCoreComponent() const { return Get<ACRenderMeshCore>(); }

	protected:
		RenderMeshUser(ECS_AssetHandle ECS_handle) : RenderMeshObserver(ECS_handle) { }
	};

	class RenderMesh final : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::RenderMeshAsset; }
		static void EmplaceCore(AssetID assetID) { AssetManager::GetRegistry().emplace<ACRenderMeshCore>(assetID).Init(); }

		using Observer = RenderMeshObserver;
		using User = RenderMeshUser;
		using Core = ACRenderMeshCore;
	};
}