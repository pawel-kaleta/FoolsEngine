#pragma once

#include "FoolsEngine\Assets\AssetHandle.h"

#include "Mesh.h"
#include "RenderMesh.h"

namespace fe
{
	enum class SkeletonType
	{
		None = 0,
		Humanoid
	};

	struct ACModelData final : public AssetComponent
	{
		std::vector<AssetID> RenderMeshes;
	};

	class Model final : public Asset
	{
	public:
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::ModelAsset; }

		virtual void PlaceCoreComponent() final override;
		virtual void Release() final override { };

		const std::vector<AssetID>& GetRenderMeshes() const { return Get<ACModelData>().RenderMeshes; }
		      std::vector<AssetID>& GetRenderMeshes()       { return Get<ACModelData>().RenderMeshes; }
		
	protected:
		Model(ECS_AssetHandle ECS_handle) : Asset(ECS_handle) {}
	private:
		friend class ModelImporter;
	};
	
}