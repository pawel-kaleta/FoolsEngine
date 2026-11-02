#include "AssetsRegistrySerialization.h"

#include "YAML.h"

#include "FoolsEngine/Foundation/Containers/Xar.h"

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetTypesList.h"
#include "FoolsEngine/Assets/AssetManager.h"
#include "FoolsEngine/TypeRegs/Assets.h"

#include <string>

namespace fe::AssetSerializer
{
	void SerializeRegistry()
	{
		Pile p;
		STD_PMR_Allocator pmr(&p);

		YAML::Emitter emitter;
		auto& reg = AssetManager::s_Instance->m_Registry;

		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Masters" << YAML::Value << YAML::BeginSeq;
		auto paths_view = reg.view<ACFilepath, ACRefsCounters>();
		for (auto id : paths_view)
		{
			auto& ac_path = paths_view.get<ACFilepath>(id).Filepath;
			auto& type = reg.get<ACAssetType>(id).Type;

			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Type" << YAML::Value << type.ToCString().Data;
			emitter << YAML::Key << "UUID" << YAML::Value << reg.get<ACUUID>(id).UUID;
			emitter << YAML::Key << "Filepath" << YAML::Value << ac_path.string<PMR_STRING_TEMPLATE_PARAMS>(&pmr);
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

		YAML::Node node;
		
		{
			FE_PROFILER_SCOPE("YAML::LoadFile");
			node = YAML::LoadFile((Project::Get()->m_AssetsPath / "AssetsRegistry.fear").string());
		}

		if (!node["Masters"])   return false;

		auto& reg = AssetManager::s_Instance->m_Registry;

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
				reg.emplace<ACAssetType>(assetID).Type.FromString(String(asset["Type"].as<std::string>().c_str()));
				reg.emplace<ACRefsCounters>(assetID);
			}
		}

		Context::Log::Core.Info("Asset Registry deserialized"_FE);

		return true;
	}

	template <typename tnAssetType>
	static void LoadMetaData()
	{
		FE_PROFILER_FUNC();

		auto& reg = AssetManager::s_Instance->m_Registry;
		auto paths_view = reg.view<ACFilepath, ACRefsCounters>();
		
		for (const auto asset_id : paths_view)
		{
			auto& type = reg.get<ACAssetType>(asset_id).Type;
			if (tnAssetType::GetTypeStatic() == type)
			{
				FE_PROFILER_SCOPE("Asset");

				tnAssetType::EmplaceCore(asset_id);




				if constexpr (tnAssetType::GetTypeStatic() == AssetType::Texture2D)
				{
					Pile p;
					STD_PMR_Allocator pmr(&p);

					auto filepath = Project::Get()->m_AssetsPath;
					auto source = paths_view.get<ACFilepath>(asset_id).Filepath;

					filepath /= source;

					YAML::Node node = YAML::LoadFile(filepath.string());

					Render::GAPI::Descriptors::TextureSpec mSpecification;
					auto& spec = reg.get<Texture2D::Core>(asset_id).mSpecification;
					spec.mDimentions.x = node["Width"].as<UInt>();
					spec.mDimentions.y = node["Height"].as<UInt>();
					auto format = node["Format"].as<std::string>();
					spec.mFormat.FromString(String(format.c_str(), format.size() + 1));
					spec.mLayerCount = 1;
					spec.mType = Render::GAPI::Descriptors::TextureType::Texture2D;
					spec.CalculateMipCount();

					auto& source_path = reg.emplace<ACSourceFilepath>(asset_id);
					source_path.Filepath = node["Source Filepath"].as<std::string>();
				}

				Context::Log::Core.Error("tnAssetType::LoadMetadata() not implemented"_FE);
				
				//bool result = tnAssetType::LoadMetadata(asset_id);

				//FE_CORE_ASSERT(result, "Failed to load asset metadata");
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

		Context::Log::Core.Info("Assets' meta data loaded"_FE);
	}
}