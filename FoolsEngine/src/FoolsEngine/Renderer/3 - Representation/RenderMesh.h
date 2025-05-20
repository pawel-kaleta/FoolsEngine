#pragma once

#include "FoolsEngine\Assets\Asset.h"

namespace fe
{
	struct ACRenderMeshData final : public AssetComponent
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
		virtual AssetType GetType() const override final { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::RenderMeshAsset; }

		const ACRenderMeshData& GetDataComponent() const { return Get<ACRenderMeshData>(); }

	protected:
		RenderMeshObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) { }
	};

	class RenderMeshUser : public RenderMeshObserver
	{
	public:
		void PlaceCoreComponent() const { Emplace<ACRenderMeshData>().Init(); };

		ACRenderMeshData& GetDataComponent() const { return Get<ACRenderMeshData>(); }

	protected:
		RenderMeshUser(ECS_AssetHandle ECS_handle) : RenderMeshObserver(ECS_handle) { }
	};

	class RenderMesh final : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::RenderMeshAsset; }

		using Observer = RenderMeshObserver;
		using User = RenderMeshUser;
	};
}