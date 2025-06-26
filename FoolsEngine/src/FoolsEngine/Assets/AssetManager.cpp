#include "FE_pch.h"
#include "AssetManager.h"

#include "AssetHandle.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "Loaders\TextureLoader.h"
#include "Loaders\ShaderLoader.h"
#include "Loaders\GeometryLoader.h"

#include "Serialization\YAML.h"

namespace fe
{
	AssetManager* AssetManager::s_Instance;

	AssetManager::AssetManager()
	{
		s_Instance = this;

		m_LoadingGroups.Unload = m_Registry.group<>(
			entt::get<ACLoadedFlag>,
			entt::exclude<
				ACLoadedAsDependence,
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
			entt::exclude<ACLoadedFlag>
		);

		m_LoadingGroups.VeryLow = m_Registry.group<>(
			entt::get<ACLoadFlag<AssetLoadingPriority::VeryLow>>,
			entt::exclude<ACLoadedFlag>
		);

		m_LoadingGroups.Low = m_Registry.group<>(
			entt::get<ACLoadFlag<AssetLoadingPriority::Low>>,
			entt::exclude<ACLoadedFlag>
		);

		m_LoadingGroups.Standard = m_Registry.group<>(
			entt::get< ACLoadFlag<AssetLoadingPriority::Standard>>,
			entt::exclude<ACLoadedFlag>
		);

		m_LoadingGroups.High = m_Registry.group<>(
			entt::get<ACLoadFlag<AssetLoadingPriority::High>>,
			entt::exclude<ACLoadedFlag>
		);

		m_LoadingGroups.VeryHigh = m_Registry.group<>(
			entt::get<ACLoadFlag<AssetLoadingPriority::VeryHigh>>,
			entt::exclude<ACLoadedFlag>
		);

		m_LoadingGroups.Critical = m_Registry.group<>(
			entt::get<ACLoadFlag<AssetLoadingPriority::Critical>>,
			entt::exclude<ACLoadedFlag>
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

	void AssetManager::EvaluateAndReload()
	{
		FE_PROFILER_FUNC();

		auto GDI = Renderer::GetActiveGDItype();
		auto& reg = s_Instance->m_Registry;

		auto view = reg.view<ACRefsCounters>();

		for (auto id : view)
		{
			auto type = reg.get<ACAssetType>(id).Type;

			FE_PROFILER_SCOPE(type.ToConstCharPtr());

			auto& ref_counters = view.get<ACRefsCounters>(id);

			switch (type.Value)
			{
			case AssetType::Texture2D:
			{
				auto textureUser = AssetUser<Texture2D>(id);
				if (ref_counters.LiveHandles == 0)
				{
					textureUser.Release();
				}
				else
				{
					if (!textureUser.GetRendererID(GDI))
					{
						TextureLoader::LoadTexture(textureUser);
						textureUser.CreateGDITexture2D(GDI);
						textureUser.UnloadFromCPU();
					}
				}
				break;
			}
			case AssetType::Shader:
			{
				auto shaderUser = AssetUser<Shader>(id);
				if (ref_counters.LiveHandles == 0)
				{
					shaderUser.Release();
				}
				else
				{
					if (!shaderUser.GetRendererID(GDI))
					{
						ShaderLoader::LoadShader(shaderUser);
						ShaderLoader::CompileShader(GDI, shaderUser);
					}
				}
				break;
			}
			case AssetType::Mesh:
			{
				auto meshUser = AssetUser<Mesh>(id);
				if (ref_counters.LiveHandles == 0)
				{
					meshUser.Release();
				}
				else
				{
					if (!meshUser.GetBuffers())
					{
						GeometryLoader::LoadMesh(meshUser);
						meshUser.SendDataToGPU(GDI);
						meshUser.UnloadFromCPU();
					}
				}
			}
			}
		}
	}

	
}