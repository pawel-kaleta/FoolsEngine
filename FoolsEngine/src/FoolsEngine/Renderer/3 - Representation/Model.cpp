#include "FE_pch.h"
#include "Model.h"

#include "RenderMesh.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

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

		auto filepath = Project::GetInstance()->AssetsPath;
		filepath /= AssetManager::GetRegistry().get<ACFilepath>(assetID).Filepath;
		YAML::Node node = YAML::LoadFile(filepath.string());
		bool success = Model::LoadMetadataInternal(assetID, node);

		if (!success) return false;

		const auto& source_filepath_node = node["Source Filepath"];
		if (!source_filepath_node) return false;

		AssetManager::SetSourcePath(assetID, source_filepath_node.as<std::string>());
	}

	bool Model::LoadMetadataInternal(AssetID assetID, const YAML::Node& node)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::GetRegistry();
		auto& core = reg.get<ACModelCore>(assetID);

		auto uuid_node = node["UUID"];
		if (uuid_node) // Base Assets don't have UUID in their file
		{
			if (reg.get<ACUUID>(assetID).UUID != node["UUID"].as<UUID>())
			{
				FE_CORE_ASSERT(false, "Not machting UUID in asset and its serialized node!");
				return false;
			}
		}
		else
		{
			FE_LOG_CORE_WARN("Missing UUID in Model serialized node");
		}

		const auto& rendermeshes_node = node["RenderMeshes"];

		for (const auto& render_mesh_node : rendermeshes_node)
		{
			const auto& render_mesh_uuid_node = render_mesh_node["UUID"];

			auto render_mesh_ID = AssetManager::GetOrCreateAssetWithUUID(render_mesh_uuid_node.as<UUID>());
			core.RenderMeshIDs.emplace_back(render_mesh_ID);

			reg.emplace<ACAssetType>(render_mesh_ID).Type = AssetType::RenderMesh;
			reg.emplace<ACMasterAsset>(render_mesh_ID).Master = assetID;
			reg.emplace<RenderMesh::Core>(render_mesh_ID).Init();

			bool success = RenderMesh::LoadMetadataInternal(render_mesh_ID, render_mesh_node);
			
			if (!success)
				return false;
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