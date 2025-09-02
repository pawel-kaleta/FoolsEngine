#include "FE_pch.h"
#include "Model.h"

#include "RenderMesh.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

#include "FoolsEngine\Core\Project.h"

namespace fe
{
	extern void EmitShaderDataType(YAML::Emitter& emitter, char* dataPtr, const ShaderData::Type& type);

	void Model::SaveMetadata(YAML::Emitter& emitter, AssetID assetID)
	{
		Scratchpad sp;

		AssetObserver<Model> assetObserver(assetID);
		auto& model_core = assetObserver.GetCoreComponent();

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << assetObserver.GetUUID();
		emitter << YAML::Key << "Filepath" << YAML::Value << assetObserver.GetSourceFilepath()->Filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp);
		emitter << YAML::Key << "RenderMeshes" << YAML::Value << YAML::BeginSeq;

		for (const auto& renderMeshID : model_core.RenderMeshIDs)
		{
			RenderMesh::SaveMetadata(emitter, renderMeshID);
		}
		emitter << YAML::EndSeq;

		emitter << YAML::EndMap;
	}

	bool Model::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		FE_LOG_CORE_WARN("Model metadata loading not implemented");
		return true;

		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		auto filepath = Project::GetInstance()->AssetsPath;
		filepath /= ECS_handle.get<ACFilepath>().Filepath;
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