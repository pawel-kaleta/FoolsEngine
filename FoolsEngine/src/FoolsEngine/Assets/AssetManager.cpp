#include "FE_pch.h"
#include "AssetManager.h"

#include "AssetHandle.h"
#include "Loaders/ShaderLoader.h"
#include "Loaders/TextureLoader.h"
#include "Loaders/GeometryLoader.h"
#include "Serialization/YAML.h"

#include "FoolsEngine/Foundation/Memory/Scratchpad.h"

#include "FoolsEngine/Renderer/5 - Representation/Material.h"
#include "FoolsEngine/Renderer/7 - Integration/Renderer.h"

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
		AssetUser<tnAsset> asset_user(id);
		asset_user.Release();
		asset_user.UnloadFromCPU();
	}

	void AssetManager::EvaluateAndReload()
	{
		FE_PROFILER_FUNC();

		auto GAPI = Renderer::GetActiveGAPIType();
		auto& reg = s_Instance->m_Registry;

		auto& groups = s_Instance->m_LoadingGroups;

		Scratchpad sp;

		std::pmr::vector<AssetID> entities_to_unload(&sp);

		entities_to_unload.reserve(groups.Unload.size());
		for (auto id : groups.Unload)
		{
			entities_to_unload.emplace_back(id);
			reg.storage<ACLoaded>().erase(id);
		}

		for (auto id : entities_to_unload)
		{
			switch (reg.get<ACAssetType>(id).Type)
			{
			case AssetType::Texture2D:	Unload<Texture2D>(id);	break;
			case AssetType::Mesh:		Unload<Mesh>(id);		break;
			case AssetType::Material:	Unload<Material>(id);	break;
			case AssetType::Model:		Unload<Model>(id);		break;
			//...
			}
		}

		std::pmr::vector<AssetID> assets_to_load(&sp);

		// this will be separated in future
		for (auto id : groups.Critical	) assets_to_load.emplace_back(id);
		for (auto id : groups.VeryHigh	) assets_to_load.emplace_back(id);
		for (auto id : groups.High		) assets_to_load.emplace_back(id);
		for (auto id : groups.Standard	) assets_to_load.emplace_back(id);
		for (auto id : groups.Low		) assets_to_load.emplace_back(id);
		for (auto id : groups.VeryLow	) assets_to_load.emplace_back(id);
		for (auto id : groups.Minimal	) assets_to_load.emplace_back(id);

		for (auto id : assets_to_load)
		{
			auto& type = reg.get<ACAssetType>(id).Type;
			switch (type)
			{
			case AssetType::Texture2D:
			{
				auto texture_user = AssetUser<Texture2D>(id);
				if (!texture_user.IsLoaded())
				{
					TextureLoader::LoadTexture(texture_user);
					FE_CORE_ASSERT(false, "not implemented");
					//auto& texture = texture_user.CreateResource<GAPIType::OpenGL>().Texture;
					//texture_user.SendDataToGPU(GAPI);
					texture_user.UnloadFromCPU();
					texture_user.FlagLoaded();
				}
				else
				{
					FE_LOG_CORE_WARN("Texture allready loaded");
				}
				break;
			}
			case AssetType::Mesh:
			{
				auto mesh_user = AssetUser<Mesh>(id);
				if (!mesh_user.GetBuffer())
				{
					GeometryLoader::LoadMesh(mesh_user);
					mesh_user.SendDataToGPU(GAPI);
					mesh_user.UnloadFromCPU();
					mesh_user.FlagLoaded();
				}
				else
				{
					FE_LOG_CORE_WARN("Mesh allready loaded");
				}
				break;
			}
			case AssetType::Material:
			{
				auto material_user = AssetUser<Material>(id);
				material_user.SendDataToGPU(GAPI);
				material_user.FlagLoaded();
				break;
			}
			case AssetType::Model:
			{
				auto model_user = AssetUser<Model>(id);
				GeometryLoader::LoadModel(model_user);
				model_user.SendDataToGPU(GAPI);
				model_user.FlagLoaded();
				break;
			}
			}
		}
	}
}