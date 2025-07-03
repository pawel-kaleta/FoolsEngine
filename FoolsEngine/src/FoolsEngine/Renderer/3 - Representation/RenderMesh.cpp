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

		if (!core.MaterialHandle.IsValid() || !core.MeshHandle.IsValid())
			return false;
		
		// material loading
		{
			auto material_user = core.MaterialHandle.Use();
			if (!material_user.AllOf<ACLoadedFlag>())
				if (!material_user.SendDataToGPU(GDI))
					return false;

			auto refs = material_user.GetRefCounters();
			if (refs)
			{
				if (refs->LiveHandles[0].fetch_add(1) == 0)
					material_user.FlagLoadedAsDependency();
			}
			else
			{
				FE_CORE_ASSERT(false, "");
			}

			material_user.FlagLoaded();
		}

		// mesh loading
		{
			auto mesh_user = core.MeshHandle.Use();
			if (!mesh_user.AllOf<ACLoadedFlag>())
				if (!mesh_user.SendDataToGPU(GDI))
					return false;

			auto refs = mesh_user.GetRefCounters();
			if (refs)
			{
				if (refs->LiveHandles[0].fetch_add(1) == 0)
					mesh_user.FlagLoadedAsDependency();
			}
			else
			{
				FE_CORE_ASSERT(false, "");
			}

			mesh_user.FlagLoaded();
		}

		return true;
	}

	void RenderMesh::SaveMetadata(AssetID assetID)
	{
		auto assetObserver = AssetObserver<RenderMesh>(assetID);
		auto& core = assetObserver.GetCoreComponent();

		YAML::Emitter emitter;

		emitter << YAML::Key << "MeshID"     << YAML::Value << core.MeshHandle.GetUUID();
		emitter << YAML::Key << "MaterialID" << YAML::Value << core.MaterialHandle.GetUUID();

		std::ofstream fout(assetObserver.GetFilepath());
		fout << emitter.c_str();
	}

	bool RenderMesh::LoadMetadata(AssetID assetID)
	{
		FE_PROFILER_FUNC();

		ECS_AssetHandle ECS_handle(AssetManager::GetRegistry(), assetID);

		auto& filepath = ECS_handle.get<ACFilepath>().Filepath;

		YAML::Node node = YAML::LoadFile(filepath.string());

		const auto& meshID_node = node["MeshID"];
		const auto& materialID_node = node["MaterialID"];

		if (!meshID_node) return false;
		if (!materialID_node) return false;

		auto& core = ECS_handle.get<ACRenderMeshCore>();
		core.MeshHandle.SetID(AssetManager::GetOrCreateAssetWithUUID(meshID_node.as<UUID>()));
		core.MaterialHandle.SetID(AssetManager::GetOrCreateAssetWithUUID(materialID_node.as<UUID>()));

		return true;
	}
}