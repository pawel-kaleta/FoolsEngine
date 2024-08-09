#include "FE_pch.h"

#include "AssetsSerializer.h"
#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetManager.h"

#include "YAML.h"

namespace fe
{
	const char* AssetSerializer::AssetTypeName(AssetType type)
	{
		const static char* names[AssetType::Count] = {
			"Scenes",
			"Textures",
			"Textures2D",
			"Meshes",
			"Models",
			"Shaders",
			"Materials",
			"MaterialInstances",
			"Audio"
		};

		return names[type];
	}

	void AssetSerializer::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Assets" << YAML::Value << YAML::BeginMap;
		for (size_t i = 0; i < (size_t)AssetType::Count; i++)
		{
			AssetType type = (AssetType)i;
			emitter << YAML::Key << AssetTypeName(type);

			auto reg = AssetManager::GetRegistry(type);
			auto view = reg->view<ACUUID, ACProxyFilepath>();
			
			emitter << YAML::Value << YAML::BeginSeq;
			for (auto id : view)
			{
				if (BaseAssets::IsBaseAsset(id, type))
					continue;

				auto [CUUID, CFilepath] = view.get(id);

				emitter << YAML::BeginMap;
				emitter << YAML::Key << "UUID" << YAML::Value << CUUID.UUID;
				emitter << YAML::Key << "Filepath" << YAML::Value << CFilepath.Filepath.string();
				emitter << YAML::EndMap;
			}
			emitter << YAML::EndSeq;
		}
		emitter << YAML::EndMap;
	}

	bool AssetSerializer::Deserialize(YAML::Node& node)
	{
		//AssetManager::ClearRegistries();

		auto& assets = node["Assets"];

		for (size_t i = 0; i < (size_t)AssetType::Count; i++)
		{
			AssetType type = (AssetType)i;
			if (!assets[AssetTypeName(type)]) return false;
			auto& assetsOfType = assets[AssetTypeName(type)];

			for (auto asset : assetsOfType)
			{
				if (!asset["UUID"])     return false;
				if (!asset["Filepath"]) return false;

				std::filesystem::path filepath = asset["Filepath"].as<std::string>();
				UUID uuid = asset["UUID"].as<UUID>();

				AssetID id = AssetManager::CreateAsset(filepath, type);

				FE_CORE_ASSERT(AssetManager::GetRegistry(type)->get<ACUUID>(id).UUID == uuid, "Asset import failed");
			}
		}

		return true;
	}
}