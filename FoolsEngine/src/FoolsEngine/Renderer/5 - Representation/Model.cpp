#include "FE_pch.h"
#include "Model.h"

#include "RenderMesh.h"

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Assets/Serialization/YAML.h"
#include "FoolsEngine/Assets/Serialization/GPUDataSerialization.h"

namespace fe
{
	//extern void EmitGPUDataType(YAML::Emitter& emitter, char* dataPtr, const Description::Data::Type& type);

	void Model::SaveMetadata(YAML::Emitter& emitter, AssetID assetID)
	{
		Scratchpad sp;

		AssetObserver<Model> asset_observer(assetID);
		auto& model_core = asset_observer.GetCore();

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << asset_observer.GetUUID();
		emitter << YAML::Key << "Source Filepath" << YAML::Value << asset_observer.GetSourceFilepath()->Filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp);
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

		auto& reg = AssetManager::Get().m_Registry;

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

	bool ModelUser::SendDataToGPU(GAPIType GAPI) const
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
						if (!rendermesh_user.SendDataToGPU(GAPI))
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

				if (!rendermesh_user.SendDataToGPU(GAPI))
					return false;
				rendermesh_user.FlagLoaded();
				rendermesh_user.FlagLoadedAsDependency();
			}
		}

		return true;
	}

	void ModelUser::Release() const
	{
		auto& core = Get<ACModelCore>();

		for (auto rendermeshID : core.RenderMeshIDs)
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