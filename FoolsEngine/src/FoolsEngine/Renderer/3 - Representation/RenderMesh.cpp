#include "FE_pch.h"
#include "RenderMesh.h"

#include "Mesh.h"
#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\3 - Representation\Material.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

namespace fe
{
	bool RenderMeshUser::SendDataToGPU(GDIType GDI) const
	{
		auto& core = Get<ACRenderMeshCore>();

		if (!core.MaterialID || !core.MeshID)
			return false;
		
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
						if (!material_user.SendDataToGPU(GDI))
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
				
				if (!material_user.SendDataToGPU(GDI))
					return false;
				material_user.FlagLoaded();
				material_user.FlagLoadedAsDependency();
			}

		}

		// mesh loading
		{
			AssetUser<Material> mesh_user(core.MeshID);

			auto refs = mesh_user.GetRefCounters();
			if (refs) // project asset
			{
				if (refs->LiveHandles[0].fetch_add(1) == 0)
				{
					if (!mesh_user.IsLoaded())
					{
						if (!mesh_user.SendDataToGPU(GDI))
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

				if (!mesh_user.SendDataToGPU(GDI))
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
		auto assetObserver = AssetObserver<RenderMesh>(assetID);
		auto& core = assetObserver.GetCoreComponent();

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "UUID" << YAML::Value << assetObserver.GetUUID();
		emitter << YAML::Key << "Mesh" << YAML::Value;
		Mesh::SaveMetadata(emitter, core.MeshID);
		emitter << YAML::Key << "Material" << YAML::Value;
		Material::SaveMetadata(emitter, core.MaterialID);
		emitter << YAML::EndMap;
	}

	bool RenderMesh::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::GetRegistry();
		auto& filepath = reg.get<ACFilepath>(assetID).Filepath;
		YAML::Node node = YAML::LoadFile(filepath.string());
		bool success = RenderMesh::LoadMetadataInternal(assetID, node);

		if (!success) return false;

		const auto& source_filepath_node = node["Source Filepath"];
		if (!source_filepath_node) return false;

		AssetManager::SetSourcePath(assetID, source_filepath_node.as<std::string>());
	}

	bool RenderMesh::LoadMetadataInternal(AssetID assetID, const YAML::Node& node)
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::GetRegistry();

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

		const auto& mesh_node = node["Mesh"];
		const auto& material_node = node["Material"];

		if (!mesh_node ||
			!material_node)
			return false;

		const auto& mesh_uuid_node = mesh_node["UUID"];
		const auto& material_uuid_node = material_node["UUID"];

		if (!mesh_uuid_node ||
			!material_uuid_node)
			return false;

		auto mesh_uuid = mesh_uuid_node.as<UUID>();
		auto material_uuid = material_uuid_node.as<UUID>();

		auto& core = reg.get<ACRenderMeshCore>(assetID);

		if (mesh_uuid == UUID(0))
			core.MeshID = NullAssetID;
		else
		{
			core.MeshID = AssetManager::GetOrCreateAssetWithUUID(mesh_uuid);
			reg.emplace<ACAssetType>(core.MeshID).Type = AssetType::Mesh;
			reg.emplace<ACMasterAsset>(core.MeshID).Master = assetID;
			reg.emplace<Mesh::Core>(core.MeshID).Init();

			bool success = Mesh::LoadMetadataInternal(core.MeshID, mesh_node);
			if (!success) return false;
		}

		if (material_uuid == UUID(0))
			core.MaterialID = NullAssetID;
		else
		{
			core.MaterialID = AssetManager::GetOrCreateAssetWithUUID(material_uuid);
			reg.emplace<ACAssetType>(core.MaterialID).Type = AssetType::Material;
			reg.emplace<ACMasterAsset>(core.MaterialID).Master = assetID;
			reg.emplace<Material::Core>(core.MaterialID).Init();

			bool success = Material::LoadMetadataInternal(core.MaterialID, material_node);
			if (!success) return false;
		}

		return true;
	}
}