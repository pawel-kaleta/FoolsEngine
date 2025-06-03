#include "FE_pch.h"
#include "Model.h"

#include "RenderMesh.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

namespace fe
{
	void Model::Serialize(const AssetObserver<Model>& assetObserver)
	{
		auto& core = assetObserver.GetCoreComponent();

		YAML::Emitter emitter;

		emitter << YAML::BeginSeq;
		for (const auto& renderMeshID : core.RenderMeshes)
		{
			emitter << AssetObserver<RenderMesh>(renderMeshID).GetUUID();
		}
		emitter << YAML::EndSeq;

		std::ofstream fout(assetObserver.GetFilepath());
		fout << emitter.c_str();
	}

	bool Model::Deserialize(AssetID assetID)
	{
		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		auto& filepath = ECS_handle.get<ACFilepath>().Filepath;

		YAML::Node node = YAML::LoadFile(filepath.string());
		auto& core = ECS_handle.get<ACModelCore>();

		for (const auto& render_mesh_node : node)
		{
			core.RenderMeshes.push_back(AssetManager::GetOrCreateAssetWithUUID(render_mesh_node.as<UUID>()));
		}

		return true;
	}
}