#include "FE_pch.h"
#include "Model.h"

#include "RenderMesh.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Assets\Serialization\ShaderDataSerialization.h"

#include "FoolsEngine\Core\Project.h"

namespace fe
{
	//extern void EmitShaderDataType(YAML::Emitter& emitter, char* dataPtr, const ShaderData::Type& type);

	void Model::SaveMetadata(YAML::Emitter& emitter, AssetID assetID)
	{
		Scratchpad sp;

		AssetObserver<Model> assetObserver(assetID);
		auto& model_core = assetObserver.GetCoreComponent();

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << assetObserver.GetUUID();
		emitter << YAML::Key << "Source Filepath" << YAML::Value << assetObserver.GetSourceFilepath()->Filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp);
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

		auto& reg = AssetManager::GetRegistry();

		const auto& filepath = reg.get<ACFilepath>(assetID).Filepath;
		auto full_filepath = Project::Get()->m_AssetsPath / filepath;

		YAML::Node node;

		{
			FE_PROFILER_SCOPE("YAML::LoadFile");
			node = YAML::LoadFile(full_filepath.string());
		}

		const auto& uuid_node = node["UUID"];
		if (uuid_node) // Base Assets don't have UUID in their file
		{
			if (reg.get<ACUUID>(assetID).UUID != node["UUID"].as<UUID>())
			{
				FE_LOG_CORE_ERROR("Not machting UUID in asset and its serialized node!");
				return false;
			}
		}
		else
		{
			FE_LOG_CORE_ERROR("Missing UUID in Model serialized node");
		}

		const auto& source_filepath_node = node["Source Filepath"];
		if (!source_filepath_node) return false;
		std::filesystem::path source_filepath = source_filepath_node.as<std::string>();
		AssetManager::SetSourcePath(assetID, source_filepath);

		const auto parent_path = filepath.parent_path();
		auto& core = reg.get<ACModelCore>(assetID);

		const auto& rendermeshes_node = node["RenderMeshes"];
		if (!rendermeshes_node) return false;
		for (const auto& render_mesh_node : rendermeshes_node)
		{
			auto render_mesh_ID = RenderMesh::LoadMetadataInternal(render_mesh_node, assetID, parent_path);
			core.RenderMeshIDs.emplace_back(render_mesh_ID);
		}

		return true;
	}

	bool ModelUser::SendDataToGPU(GDIType GDI) const
	{
		auto& ACData = Get<ACModelCore>();

		for (auto rendermeshID : ACData.RenderMeshIDs)
		{
			AssetUser<RenderMesh> rendermesh_user(rendermeshID);

			auto refs = rendermesh_user.GetRefCounters();
			if (refs) // project asset
			{
				if (refs->LiveHandles[0].fetch_add(1) == 0)
				{
					if (!rendermesh_user.IsLoaded())
					{
						if (!rendermesh_user.SendDataToGPU(GDI))
							return false;

						rendermesh_user.FlagLoaded();
					}

					rendermesh_user.FlagLoadedAsDependency();
				}
			}
			else // internal asset
			{
				FE_CORE_ASSERT(!rendermesh_user.IsLoadedAsDependency(), "Internal RenderMesh already marked LoadedAsDependency during loading");
				FE_CORE_ASSERT(!rendermesh_user.IsLoaded(), "Internal RenderMesh already marked Loaded during loading");

				if (!rendermesh_user.SendDataToGPU(GDI))
					return false;
				rendermesh_user.FlagLoaded();
				rendermesh_user.FlagLoadedAsDependency();
			}
		}

		return true;
	}

	void ModelUser::Release() const
	{
		auto& ACData = Get<ACModelCore>();

		for (auto rendermeshID : ACData.RenderMeshIDs)
		{
			AssetUser<RenderMesh> rendermesh_user(rendermeshID);

			auto refs = rendermesh_user.GetRefCounters();
			if (refs) // project asset
			{
				if (refs->LiveHandles[0].fetch_sub(1) == 1)
					rendermesh_user.ReleaseDependencyLoad();
			}
			else // internal asset
			{
				rendermesh_user.ReleaseDependencyLoad();
			}
		}
	}
}