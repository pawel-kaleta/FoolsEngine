#pragma once

#include "FoolsEngine\Assets\AssetHandle.h"

#include "Mesh.h"
#include "RenderMesh.h"

namespace fe
{
	struct ACModelData final : public AssetComponent
	{
		std::vector<AssetID> RenderMeshes;
	};

	class ModelObserver : public AssetInterface
	{
	public:
		virtual AssetType GetType() const override final { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::ModelAsset; }

		const std::vector<AssetID>& GetRenderMeshes() const { return Get<ACModelData>().RenderMeshes; }
		
	protected:
		ModelObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};
	
	class ModelUser : public ModelObserver
	{
	public:
		void PlaceCoreComponent() const;

		std::vector<AssetID>& GetRenderMeshes() const { return Get<ACModelData>().RenderMeshes; }

	protected:
		ModelUser(ECS_AssetHandle ECS_handle) : ModelObserver(ECS_handle) {}
	};

	class Model : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::ModelAsset; }

		using Observer = ModelObserver;
		using User = ModelUser;
	};
}