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

		FE_LOG_CORE_WARN("Model metadata loading not implemented");
		return true;

		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		auto& filepath = ECS_handle.get<ACFilepath>().Filepath;

		YAML::Node node = YAML::LoadFile(filepath.string());

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
			FE_LOG_CORE_WARN("Missing UUID in RenderMesh file");
		}

		const auto& meshID_node = node["MeshID"];
		const auto& materialID_node = node["MaterialID"];

		if (!meshID_node) return false;
		if (!materialID_node) return false;

		auto& core = ECS_handle.get<ACRenderMeshCore>();
		core.MeshID = AssetManager::GetOrCreateAssetWithUUID(meshID_node.as<UUID>());
		core.MaterialID = AssetManager::GetOrCreateAssetWithUUID(materialID_node.as<UUID>());

		return true;
	}
}