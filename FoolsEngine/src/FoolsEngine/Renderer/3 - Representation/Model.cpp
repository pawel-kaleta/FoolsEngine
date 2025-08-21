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

		FE_LOG_CORE_ERROR("Not implemented");

		emitter << YAML::BeginSeq;
		for (const auto& renderMeshID : core.RenderMeshIDs)
		{
			emitter << renderMeshID;
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

		auto uuid_node = node["UUID"];
		if (uuid_node) // Base Assets don't have UUID in their file
		{
			if (ECS_handle.get<ACUUID>().UUID != node["UUID"].as<UUID>())
			{
				FE_CORE_ASSERT(false, "Not machting UUID in asset and its metafile!");
				return false;
			}
		}
		else
		{
			FE_LOG_CORE_WARN("Missing UUID in Model file");
		}

		for (const auto& render_mesh_node : node)
		{
			auto render_mesh_ID = AssetManager::GetOrCreateAssetWithUUID(render_mesh_node.as<UUID>());
			core.RenderMeshIDs.emplace_back(render_mesh_ID);
		}

		return true;
	}
}