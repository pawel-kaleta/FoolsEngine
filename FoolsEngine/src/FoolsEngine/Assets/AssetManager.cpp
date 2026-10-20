#include "AssetManager.h"

#include "AssetHandle.h"
#include "Loaders/ShaderLoader.h"
#include "Loaders/TextureLoader.h"
#include "Loaders/GeometryLoader.h"
#include "Serialization/YAML.h"

#include "FoolsEngine/Foundation/Context/Pile.h"
#include "FoolsEngine/Foundation/Containers/Arena.h"

#include "FoolsEngine/Renderer/2 - Representation/Material.h"
#include "FoolsEngine/Renderer/5 - Integration/Renderer.h"

namespace fe
{
	AssetManager* AssetManager::s_Instance;

	AssetManager::AssetManager()
	{
		s_Instance = this;

		m_LoadingGroups.Unload = m_Registry.group<>(
			entt::get<ACLoaded>,
			entt::exclude<
				ACLoadFlag<AssetLoadingPriority::Minimal>,
				ACLoadFlag<AssetLoadingPriority::VeryLow>,
				ACLoadFlag<AssetLoadingPriority::Low>,
				ACLoadFlag<AssetLoadingPriority::Standard>,
				ACLoadFlag<AssetLoadingPriority::High>,
				ACLoadFlag<AssetLoadingPriority::VeryHigh>,
				ACLoadFlag<AssetLoadingPriority::Critical>
			>
		);

		m_LoadingGroups.Minimal = m_Registry.group<>(
			entt::get<ACLoadFlag<AssetLoadingPriority::Minimal>>,
			entt::exclude<ACLoaded>
		);

		m_LoadingGroups.VeryLow = m_Registry.group<>(
			entt::get<ACLoadFlag<AssetLoadingPriority::VeryLow>>,
			entt::exclude<ACLoaded>
		);

		m_LoadingGroups.Low = m_Registry.group<>(
			entt::get<ACLoadFlag<AssetLoadingPriority::Low>>,
			entt::exclude<ACLoaded>
		);

		m_LoadingGroups.Standard = m_Registry.group<>(
			entt::get< ACLoadFlag<AssetLoadingPriority::Standard>>,
			entt::exclude<ACLoaded>
		);

		m_LoadingGroups.High = m_Registry.group<>(
			entt::get<ACLoadFlag<AssetLoadingPriority::High>>,
			entt::exclude<ACLoaded>
		);

		m_LoadingGroups.VeryHigh = m_Registry.group<>(
			entt::get<ACLoadFlag<AssetLoadingPriority::VeryHigh>>,
			entt::exclude<ACLoaded>
		);

		m_LoadingGroups.Critical = m_Registry.group<>(
			entt::get<ACLoadFlag<AssetLoadingPriority::Critical>>,
			entt::exclude<ACLoaded>
		);
	}

	AssetID AssetManager::GetOrCreateAssetWithUUID(UUID uuid)
	{
		auto& inst = *s_Instance;
		auto result = inst.m_MapByUUID.find(uuid);
		if (result == inst.m_MapByUUID.end())
		{
			auto assetID = inst.m_Registry.create();
			inst.m_Registry.emplace<ACUUID>(assetID).UUID = uuid;
			inst.m_MapByUUID[uuid] = assetID;
			return assetID;
		}

		return result->second;
	}

	AssetID AssetManager::GetAssetFromFilepath(const std::filesystem::path& filepath)
	{
		auto& map = s_Instance->m_MapByFilepath;
		auto search_result = map.find(filepath);
		if (search_result != map.end())
			return search_result->second;
		return NullAssetID;
	}

	const std::vector<AssetID>* AssetManager::GetAssetsFromSourceFilepath(const std::filesystem::path& filepath)
	{
		auto& map = s_Instance->m_SourceFileRegistry;
		auto search_result = map.find(filepath);
		if (search_result != map.end())
			return &(search_result->second);
	
		return nullptr;
	}

	void AssetManager::SetFilepath(AssetID assetID, const std::filesystem::path& filepath)
	{
		auto& inst = *s_Instance;
		AssetRegistry& reg = inst.m_Registry;
		auto ac_path = reg.try_get<ACFilepath>(assetID);

		if (!ac_path)
		{
			ac_path = &(reg.emplace<ACFilepath>(assetID));
		}

		ac_path->Filepath = filepath;
		inst.m_MapByFilepath[ac_path->Filepath] = assetID;
	}

	void AssetManager::SetSourcePath(AssetID assetID, const std::filesystem::path& sourcePath)
	{
		auto& inst = *s_Instance;
		AssetRegistry& reg = inst.m_Registry;
		auto ac_path = reg.try_get<ACSourceFilepath>(assetID);
		if (ac_path)
		{
			auto& assets = inst.m_SourceFileRegistry[ac_path->Filepath];
			for (auto it = assets.begin(); it != assets.end(); it++)
			{
				if (*it == assetID)
				{
					assets.erase(it);
					break;
				}
			}
		}
		else
		{
			ac_path = &(reg.emplace<ACSourceFilepath>(assetID));
		}

		ac_path->Filepath = sourcePath;
		inst.m_SourceFileRegistry[sourcePath].push_back(assetID);
	}

	template <typename tnAsset>
	void Unload(AssetID id)
	{
		FE_CORE_ASSERT(false, "Not implemented");
		AssetUser asset_user(id);
	}

	void AssetManager::EvaluateAndReload()
	{
		FE_PROFILER_FUNC();

		auto GAPI = Renderer::GetActivePlatform();
		auto& reg = s_Instance->m_Registry;

		auto& groups = s_Instance->m_LoadingGroups;

		Pile p;

		DynamicArenaSA<AssetID, Pile> assets_to_unload;
		assets_to_unload.Init(&p);

		assets_to_unload.ReserveExact(groups.Unload.size());
		for (auto id : groups.Unload)
		{
			assets_to_unload.Append(id);
		}

		for (auto id : assets_to_unload)
		{
			reg.storage<ACLoaded>().erase(id);

			switch (reg.get<ACAssetType>(id).Type)
			{
			case AssetType::Texture2D:	Unload<Texture2D>(id);	break;
			case AssetType::Mesh:		Unload<Mesh>(id);		break;
			case AssetType::Material:	Unload<Material>(id);	break;
			case AssetType::Model:		Unload<Model>(id);		break;
			//...
			}
		}

		DynamicArenaSA<AssetID, Pile> assets_to_load;
		assets_to_load.Init(&p);

		// this will be separated in future
		for (auto id : groups.Critical	) assets_to_load.Append(id);
		for (auto id : groups.VeryHigh	) assets_to_load.Append(id);
		for (auto id : groups.High		) assets_to_load.Append(id);
		for (auto id : groups.Standard	) assets_to_load.Append(id);
		for (auto id : groups.Low		) assets_to_load.Append(id);
		for (auto id : groups.VeryLow	) assets_to_load.Append(id);
		for (auto id : groups.Minimal	) assets_to_load.Append(id);

		for (auto id : assets_to_load)
		{
			auto& type = reg.get<ACAssetType>(id).Type;
			switch (type)
			{
			case AssetType::Texture2D:
			{
				auto texture_user = AssetUser(id);
				if (!texture_user.IsLoaded())
				{
					TextureLoader::LoadTexture(texture_user);
					FE_CORE_ASSERT(false, "not implemented");
					//auto& texture = texture_user.CreateResource<GAPIType::OpenGL>().Texture;
					//texture_user.SendDataToGPU(GAPI);
					//texture_user.UnloadFromCPU();
					texture_user.FlagLoaded();
				}
				else
				{
					Context::Log::Core.Warn("Texture allready loaded"_FE);
				}
				break;
			}
			case AssetType::Mesh:
			{
				auto mesh_user = AssetUser(id);
				FE_CORE_ASSERT(false, "not implemented");
				//if (!mesh_user.GetBuffer())
				//{
				//	GeometryLoader::LoadMesh(mesh_user);
				//	mesh_user.SendDataToGPU(GAPI);
				//	mesh_user.UnloadFromCPU();
				//	mesh_user.FlagLoaded();
				//}
				//else
				//{
				//	FE_LOG_CORE_WARN("Mesh allready loaded");
				//}
				break;
			}
			case AssetType::Material:
			{
				FE_CORE_ASSERT(false, "not implemented");
				auto material_user = AssetUser(id);
				//material_user.SendDataToGPU(GAPI);
				material_user.FlagLoaded();
				break;
			}
			case AssetType::Model:
			{
				FE_CORE_ASSERT(false, "not implemented");
				auto model_user = AssetUser(id);
				GeometryLoader::LoadModel(model_user);
				//model_user.SendDataToGPU(GAPI);
				model_user.FlagLoaded();
				break;
			}
			}
		}
	}
}