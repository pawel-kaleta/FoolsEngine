#include "FE_pch.h"
#include "AssetsRegistrySerialization.h"
#include "YAML.h"

#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetManager.h"

#include "FoolsEngine\Assets\AssetTypesRegistry.h"

#include <string>

namespace fe::AssetSerializer
{
	void SerializeRegistry(const std::filesystem::path& filepath)
	{
		YAML::Emitter emitter;
		auto& reg = AssetManager::GetRegistry();

		emitter << YAML::Key << "Masters" << YAML::Value << YAML::BeginSeq;
		auto paths_view = reg.view<ACFilepath>();
		for (auto id : paths_view)
		{
			auto [acpath] = paths_view.get(id);
			auto& type = reg.get<ACAssetType>(id).Type;

			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Type" << YAML::Value << type.ToString();
			emitter << YAML::Key << "UUID" << YAML::Value << reg.get<ACUUID>(id).UUID;
			emitter << YAML::Key << "Filepath" << YAML::Value << acpath.Filepath.string();
			emitter << YAML::EndMap;
		}
		emitter << YAML::EndSeq;

		emitter << YAML::Key << "Internals" << YAML::Value << YAML::BeginSeq;
		auto masters_view = reg.view<ACMasterAsset>();
		for (auto id : masters_view)
		{
			auto [acmaster] = masters_view.get(id);
			auto& type = reg.get<ACAssetType>(id).Type;

			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Type" << YAML::Value << type.ToString();
			emitter << YAML::Key << "UUID" << YAML::Value << reg.get<ACUUID>(id).UUID;
			emitter << YAML::Key << "Master" << YAML::Value << reg.get<ACUUID>(acmaster.Master).UUID;
			emitter << YAML::EndMap;
		}
		emitter << YAML::EndSeq;

		std::ofstream fout(filepath);
		fout << emitter.c_str();
	}

	bool DeserializeRegistry(const std::filesystem::path& filepath)
	{
		FE_PROFILER_FUNC();

		YAML::Node node = YAML::LoadFile((filepath/"AssetsRegistry.fear").string());
		if (!node["Masters"])   return false;
		if (!node["Internals"]) return false;

		auto& reg = AssetManager::GetRegistry();

		{
			FE_PROFILER_SCOPE("Masters");
			for (auto asset : node["Masters"])
			{
				FE_PROFILER_SCOPE("Asset");
				if (!asset["Type"])     return false;
				if (!asset["UUID"])     return false;
				if (!asset["Filepath"]) return false;

				AssetID assetID = AssetManager::GetOrCreateAssetWithUUID(asset["UUID"].as<UUID>());
				AssetManager::SetFilepath(assetID, asset["Filepath"].as<std::string>());
				auto& debug = reg.emplace<ACAssetType>(assetID);
				debug.Type.FromString(asset["Type"].as<std::string>());
				reg.emplace<ACRefsCounters>(assetID);
			}
		}


		{
			FE_PROFILER_SCOPE("Internals");
			for (auto asset : node["Internals"])
			{
				FE_PROFILER_SCOPE("Asset");
				if (!asset["Type"])     return false;
				if (!asset["UUID"])     return false;
				if (!asset["Master"])   return false;

				AssetID assetID = AssetManager::GetOrCreateAssetWithUUID(asset["UUID"].as<UUID>());
				reg.emplace<ACAssetType>(assetID).Type.FromString(asset["Type"].as<std::string>());
				reg.emplace<ACMasterAsset>(assetID).Master = AssetManager::GetOrCreateAssetWithUUID(asset["Master"].as<UUID>());
			}
		}

		return true;
	}

	void LoadMetaData()
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::GetRegistry();
		auto paths_view = reg.view<ACFilepath>();

		for (auto assetID : paths_view)
		{
			FE_PROFILER_SCOPE("Asset");

			auto [cfilepath] = paths_view.get(assetID);
			auto type = reg.get<ACAssetType>(assetID).Type;

			for (auto& item : AssetTypesRegistry::GetItems())
			{
				if (item.Type != type)
					continue;

				(*item.EmplaceCore)(assetID);
				FE_CORE_ASSERT((*item.LoadMetadata)(assetID), "Failed to load asset metadata");

				break;
			}
		}
	}
}