#pragma once

#include "FoolsEngine\Assets\Asset.h"

namespace fe
{
	struct ACRenderMeshData final : public AssetComponent
	{
		AssetID MeshID;
		AssetID MaterialInstanceID;

		void Init()
		{
			MeshID = NullAssetID;
			MaterialInstanceID = NullAssetID;
		}
	};

	class RenderMesh final : public Asset
	{
	public:
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::RenderMeshAsset; }

		virtual void PlaceCoreComponents() final override { Emplace<ACRenderMeshData>().Init(); };
		virtual void Release() final override { };

		const ACRenderMeshData& GetData() const { return Get<ACRenderMeshData>(); }
		      ACRenderMeshData& GetData()       { return Get<ACRenderMeshData>(); }

	protected:
		RenderMesh(ECS_AssetHandle ECS_handle) : Asset(ECS_handle) { }
	};
}