#include "FE_pch.h"
#include "Model.h"

#include "RenderMesh.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

namespace fe
{
	void Model::SaveMetadata(AssetID assetID)
	{
		auto assetObserver = AssetObserver<Model>(assetID);
		auto& core = assetObserver.GetCoreComponent();

		YAML::Emitter emitter;

		emitter << YAML::BeginSeq;
		for (const auto& renderMeshHandle : core.RenderMeshes)
		{
			emitter << renderMeshHandle.GetUUID();
		}
		emitter << YAML::EndSeq;

		std::ofstream fout(assetObserver.GetFilepath());
		fout << emitter.c_str();
	}

	bool Model::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		auto& filepath = ECS_handle.get<ACFilepath>().Filepath;

		YAML::Node node = YAML::LoadFile(filepath.string());
		auto& core = ECS_handle.get<ACModelCore>();

		for (const auto& render_mesh_node : node)
		{
			auto render_mesh_ID = AssetManager::GetOrCreateAssetWithUUID(render_mesh_node.as<UUID>());
			core.RenderMeshes.emplace_back(render_mesh_ID, AssetLoadingPriority::None);
		}

		return true;
	}
}