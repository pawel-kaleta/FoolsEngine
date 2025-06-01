#include "FE_pch.h"
#include "AssetsSerializer.h"
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
		YAML::Node node = YAML::LoadFile(filepath.string());
		if (!node["Masters"])   return false;
		if (!node["Internals"]) return false;

		auto& reg = AssetManager::GetRegistry();

		for (auto asset : node["Masters"])
		{
			if (!asset["TypeName"]) return false;
			if (!asset["Type"])     return false;
			if (!asset["UUID"])     return false;
			if (!asset["Filepath"]) return false;

			AssetID assetID = AssetManager::GetOrCreateAssetWithUUID(asset["UUID"].as<UUID>());
			AssetManager::SetFilepath(assetID, asset["Filepath"].as<std::string>());
			reg.emplace<ACAssetType>(assetID).Type.FromString(asset["Type"].as<std::string>());
			reg.emplace<ACRefsCounters>(assetID);
		}

		for (auto asset : node["Internals"])
		{
			if (!asset["TypeName"]) return false;
			if (!asset["Type"])     return false;
			if (!asset["UUID"])     return false;
			if (!asset["Master"])   return false;

			AssetID assetID = AssetManager::GetOrCreateAssetWithUUID(asset["UUID"].as<UUID>());
			reg.emplace<ACAssetType>(assetID).Type.FromString(asset["Type"].as<std::string>());
			reg.emplace<ACMasterAsset>(assetID).Master = AssetManager::GetOrCreateAssetWithUUID(asset["Master"].as<UUID>());
		}

		return true;
	}

	void LoadMetaData()
	{
		auto& reg = AssetManager::GetRegistry();
		auto paths_view = reg.view<ACFilepath>();

		for (auto assetID : paths_view)
		{
			auto [cfilepath] = paths_view.get(assetID);
			auto type = reg.get<ACAssetType>(assetID).Type;

			for (auto& item : AssetTypesRegistry::GetItems())
			{
				if (item.Type != type)
					continue;

				(*item.EmplaceCore)(assetID);
				(*item.LoadMetadata)(assetID);
			}
		}
	}
}