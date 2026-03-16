#include "FE_pch.h"
#include "AssetsRegistrySerialization.h"

#include "YAML.h"

#include "FoolsEngine/Foundation/Utils/Xar.h"

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetTypes.h"
#include "FoolsEngine/Assets/AssetManager.h"
#include "FoolsEngine/Assets/AssetTypesRegistry.h"

#include <string>

namespace fe::AssetSerializer
{
	void SerializeRegistry()
	{
		Scratchpad sp;
		YAML::Emitter emitter;
		auto& reg = AssetManager::Get().m_Registry;

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Masters" << YAML::Value << YAML::BeginSeq;
		auto paths_view = reg.view<ACFilepath, ACRefsCounters>();
		for (auto id : paths_view)
		{
			auto& ac_path = paths_view.get<ACFilepath>(id).Filepath;
			auto& type = reg.get<ACAssetType>(id).Type;

			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Type" << YAML::Value << type.ToConstCharPtr();
			emitter << YAML::Key << "UUID" << YAML::Value << reg.get<ACUUID>(id).UUID;
			emitter << YAML::Key << "Filepath" << YAML::Value << ac_path.string<PMR_STRING_TEMPLATE_PARAMS>(&sp);
			emitter << YAML::EndMap;
		}
		emitter << YAML::EndSeq;
		emitter << YAML::EndMap;

		std::ofstream fout(Project::Get()->m_AssetsPath / "AssetsRegistry.fear");
		fout << emitter.c_str();
	}

	bool DeserializeRegistry()
	{
		FE_PROFILER_FUNC();

		Scratchpad sp;

		YAML::Node node;
		
		{
			FE_PROFILER_SCOPE("YAML::LoadFile");
			node = YAML::LoadFile((Project::Get()->m_AssetsPath / "AssetsRegistry.fear").string());
		}

		if (!node["Masters"])   return false;

		auto& reg = AssetManager::Get().m_Registry;

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
				reg.emplace<ACAssetType>(assetID).Type.FromString(asset["Type"].as<std::string>());
				reg.emplace<ACRefsCounters>(assetID);
			}
		}

		FE_LOG_CORE_INFO("Asset Registry deserialized");

		return true;
	}

	template <typename tnAssetType>
	static void LoadMetaData()
	{
		FE_PROFILER_FUNC();

		Scratchpad sp;

		auto& reg = AssetManager::Get().m_Registry;
		auto paths_view = reg.view<ACFilepath, ACRefsCounters>();
		
		for (const auto asset_id : paths_view)
		{

			auto& type = reg.get<ACAssetType>(asset_id).Type;
			if (tnAssetType::GetTypeStatic() == type)
			{
				FE_PROFILER_SCOPE("Asset");

				tnAssetType::EmplaceCore(asset_id);
				bool result = tnAssetType::LoadMetadata(asset_id);

				FE_CORE_ASSERT(result, "Failed to load asset metadata");
			}
		}
	}

	void LoadMetaData()
	{
		FE_PROFILER_FUNC();

		// order important becouse of cross dependencies between assets
		LoadMetaData<Shader>();
		LoadMetaData<ShadingModel>();
		LoadMetaData<Texture2D>();
		LoadMetaData<Material>();
		LoadMetaData<Mesh>();
		LoadMetaData<RenderMesh>();
		LoadMetaData<Model>();
		LoadMetaData<Scene>();

		FE_LOG_CORE_INFO("Assets' meta data loaded");
	}
}