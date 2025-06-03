#include "FE_pch.h"
#include "RenderMesh.h"

#include "Mesh.h"
#include "FoolsEngine\Renderer\3 - Representation\Material.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

namespace fe
{
	void RenderMesh::Serialize(const AssetObserver<RenderMesh>& assetObserver)
	{
		auto& core = assetObserver.GetCoreComponent();

		YAML::Emitter emitter;

		emitter << YAML::Key << "MeshID"     << YAML::Value << AssetObserver<Mesh    >(core.MeshID    ).GetUUID();
		emitter << YAML::Key << "MaterialID" << YAML::Value << AssetObserver<Material>(core.MaterialID).GetUUID();

		std::ofstream fout(assetObserver.GetFilepath());
		fout << emitter.c_str();
	}

	bool RenderMesh::Deserialize(AssetID assetID)
	{
		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		auto& filepath = ECS_handle.get<ACFilepath>().Filepath;

		YAML::Node node = YAML::LoadFile(filepath.string());

		const auto& meshID_node = node["MeshID"];
		const auto& materialID_node = node["MaterialID"];

		if (!meshID_node) return false;
		if (!materialID_node) return false;

		auto& core = ECS_handle.get<ACRenderMeshCore>();
		core.MeshID     = AssetManager::GetOrCreateAssetWithUUID(meshID_node.as<UUID>());
		core.MaterialID = AssetManager::GetOrCreateAssetWithUUID(materialID_node.as<UUID>());

		return true;
	}
}