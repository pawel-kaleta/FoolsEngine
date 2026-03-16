#include "FE_pch.h"
#include "RenderMesh.h"

#include "Mesh.h"

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Assets/Serialization/YAML.h"
#include "FoolsEngine/Assets/Serialization/GPUDataSerialization.h"

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/4 - Representation/Material.h"

namespace fe
{
	bool RenderMeshUser::SendDataToGPU(GAPIType GAPI) const
	{
		auto& core = Get<ACRenderMeshCore>();

		if (!core.MaterialID || !core.MeshID)
			return false;
	}

	bool RenderMeshUser::SendDataToGPUInternal(GAPIType GAPI, Resource::StaticBufferBase* buffer, uint32_t offset) const
	{
		auto& core = Get<ACRenderMeshCore>();


		// material loading
		{
			AssetUser<Material> material_user(core.MaterialID);

			auto refs = material_user.GetRefCounters();
			if (refs) // Project asset
			{
				if (refs->LiveHandles[0].fetch_add(1) == 0)
				{
					if (!material_user.IsLoaded())
					{
						if (!material_user.SendDataToGPUInternal(GAPI, buffer, offset))
							return false;

						material_user.FlagLoaded();
					}

					material_user.FlagLoadedAsDependency();
				}
			}
			else // internal asset
			{
				FE_CORE_ASSERT(!material_user.IsLoadedAsDependency(), "Internal Material already marked LoadedAsDependency during loading");
				FE_CORE_ASSERT(!material_user.IsLoaded(), "Internal Material already marked Loaded during loading");

				if (!material_user.SendDataToGPUInternal(GAPI, buffer, offset))
					return false;
				material_user.FlagLoaded();
				material_user.FlagLoadedAsDependency();
			}

		}

		// mesh loading
		{
			AssetUser<Mesh> mesh_user(core.MeshID);

			auto refs = mesh_user.GetRefCounters();
			if (refs) // project asset
			{
				if (refs->LiveHandles[0].fetch_add(1) == 0)
				{
					if (!mesh_user.IsLoaded())
					{
						if (!mesh_user.SendDataToGPU(GAPI))
							return false;

						mesh_user.FlagLoaded();
					}
					mesh_user.FlagLoadedAsDependency();
				}
			}
			else // internal asset
			{
				FE_CORE_ASSERT(!mesh_user.IsLoadedAsDependency(), "Internal Mesh already marked LoadedAsDependency during loading");
				FE_CORE_ASSERT(!mesh_user.IsLoaded(), "Internal Mesh already marked Loaded during loading");

				if (!mesh_user.SendDataToGPU(GAPI))
					return false;
				mesh_user.FlagLoaded();
				mesh_user.FlagLoadedAsDependency();
			}
		}

		return true;
	}

	void RenderMeshUser::Release() const
	{
		auto& core = Get<ACRenderMeshCore>();

		if (!core.MaterialID)
		{
			AssetUser<Material> material_user(core.MaterialID);

			auto refs = material_user.GetRefCounters();
			if (refs) // project asset
			{
				if (refs->LiveHandles[0].fetch_sub(1) == 1)
					material_user.ReleaseDependencyLoad();
			}
			else // internal asset
			{
				material_user.ReleaseDependencyLoad();
			}
		}

		if (!core.MeshID)
		{
			AssetUser<Material> mesh_user(core.MeshID);

			auto refs = mesh_user.GetRefCounters();
			if (refs) // project asset
			{
				if (refs->LiveHandles[0].fetch_sub(1) == 1)
					mesh_user.ReleaseDependencyLoad();
			}
			else // internal asset
			{
				mesh_user.ReleaseDependencyLoad();
			}
		}
	}

	void RenderMesh::SaveMetadata(YAML::Emitter& emitter, AssetID assetID)
	{
		auto asset_observer = AssetObserver<RenderMesh>(assetID);
		auto& core = asset_observer.GetCore();

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << asset_observer.GetUUID();
		emitter << YAML::Key << "Mesh" << YAML::Value;
		Mesh::SaveMetadata(emitter, core.MeshID);
		emitter << YAML::Key << "Material" << YAML::Value;
		Material::SaveMetadata(emitter, core.MaterialID);
		emitter << YAML::EndMap;
	}

	bool RenderMesh::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::Get().m_Registry;

		const auto& filepath = reg.get<ACFilepath>(assetID).Filepath;
		auto full_filepath = Project::Get()->m_AssetsPath /= filepath;

		YAML::Node node;

		{
			FE_PROFILER_SCOPE("YAML::LoadFile");
			node = YAML::LoadFile(full_filepath.string());
		}

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
			FE_LOG_CORE_WARN("Missing UUID in RenderMesh serialized node");
		}

		const auto& source_filepath_node = node["Source Filepath"];
		if (!source_filepath_node) return false;
		std::filesystem::path source_filepath = source_filepath_node.as<std::string>();
		AssetManager::SetSourcePath(assetID, source_filepath);

		auto& core = reg.get<RenderMesh::Core>(assetID);

		const auto& mesh_node = node["Mesh"];
		const auto& material_node = node["Material"];
		if (!mesh_node || !material_node) return false;

		const auto parent_path = filepath.parent_path();
		core.MeshID		=     Mesh::LoadMetadataInternal(    mesh_node, assetID, parent_path);
		core.MaterialID = Material::LoadMetadataInternal(material_node, assetID, parent_path);

		AssetObserver<Material> material_observer(core.MaterialID);
		AssetObserver<Mesh>		    mesh_observer(core.MeshID);
		//core.DataSize = material_observer.GetDataSize() + mesh_observer.GetDataSize();

		return true;
	}

	AssetID RenderMesh::LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::Get().m_Registry;

		const auto& uuid_node = node["UUID"];
		if (!uuid_node)
		{
			FE_LOG_CORE_WARN("Missing UUID in RenderMesh serialized node!");
			return NullAssetID;
		}
		auto uuid = uuid_node.as<UUID>();
		if (uuid == UUID(0))
		{
			FE_LOG_CORE_WARN("Missing RenderMesh definition");
			return NullAssetID;
		}

		auto asset_id = AssetManager::GetOrCreateAssetWithUUID(uuid);
		if (reg.all_of<ACRefsCounters>(asset_id)) return asset_id; // is ProjectAsset ?

		if (asset_id == 24)
		{
			int test = 0;
		}

		reg.emplace<ACAssetType>(asset_id).Type = AssetType::RenderMesh;
		reg.emplace<ACMasterAsset>(asset_id).Master = master;
		auto& core = reg.emplace<RenderMesh::Core>(asset_id);
		core.Init();

		const auto& mesh_node = node["Mesh"];
		if (!mesh_node)
		{
			FE_LOG_CORE_WARN("Missing Mesh definition in RenderMesh");
			core.MeshID = NullAssetID;
		}
		else
		{
			core.MeshID = Mesh::LoadMetadataInternal(mesh_node, master, parentPath);
			//core.DataSize += AssetObserver<Mesh>(core.MeshID).GetDataSize();
		}

		const auto& material_node = node["Material"];
		if (!material_node)
		{
			FE_LOG_CORE_WARN("Missing Material definition in RenderMesh");
			core.MaterialID = NullAssetID;
		}
		else
		{
			core.MaterialID = Material::LoadMetadataInternal(material_node, master, parentPath);
			//core.DataSize += AssetObserver<Material>(core.MaterialID).GetDataSize();
		}

		return asset_id;
	}
}