#include "FE_pch.h"
#include "AssetManager.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "Loaders\TextureLoader.h"
#include "Loaders\ShaderLoader.h"
#include "Loaders\MeshLoader.h"

#include "Serializers\YAML.h"

namespace fe
{
	AssetManager* AssetManager::s_Instance;

	AssetID AssetManager::CreateAsset(const std::filesystem::path& proxyFilePath, AssetType type)
	{
		FE_PROFILER_FUNC();

		AssetID assetID = NewID(type);
		AssetRegistry& reg = *GetRegistry(type);

		reg.emplace<ACRefsCounters>(assetID);
		reg.emplace<ACDataLocation>(assetID);

		auto& assetMaps = s_Instance->m_AssetMaps[type];

		YAML::Node node = YAML::LoadFile(proxyFilePath.string());

		UUID uuid = node["UUID"].as<UUID>();
		reg.emplace<ACUUID>(assetID).UUID = uuid;
		assetMaps.ByUUID[uuid] = assetID;

		reg.emplace<ACProxyFilepath>(assetID).Filepath = proxyFilePath;
		assetMaps.ByProxyPath[proxyFilePath] = assetID;

		auto& sourcesMap = s_Instance->m_AssetSourceMaps.ByFilepath;
		if (sourcesMap.find(proxyFilePath) != sourcesMap.end())
		{
			auto assetSourceIndex = sourcesMap.at(proxyFilePath);
			auto& assetSource = s_Instance->m_AssetSources[assetSourceIndex];

			assetSource.Assets.push_back( {assetID, type} );
			reg.emplace<ACSourceIndex>(assetID).Index = assetSourceIndex;
		}
		else
		{
			auto& assetSource = s_Instance->m_AssetSources.emplace_back();
			assetSource.Assets.push_back({ assetID, type });
			reg.emplace<ACSourceIndex>(assetID).Index = (uint32_t)s_Instance->m_AssetSources.size() - 1;

			assetSource.FilePath = node["Source File"].as<std::string>();
		}

		return assetID;
	}

	AssetID AssetManager::GetAssetWithUUID(UUID uuid, AssetType type)
	{
		FE_PROFILER_FUNC();

		if (0 == uuid)
			return NullAssetID;

		auto& assetMap = s_Instance->m_AssetMaps[type].ByUUID;

		if (assetMap.find(uuid) == assetMap.end())
			return NullAssetID;

		return assetMap.at(uuid);
	}

	AssetID AssetManager::NewBaseAsset(AssetID requestID, const std::string& name, AssetType type)
	{
		AssetID assetID = NewID(type, requestID);

		FE_CORE_ASSERT(requestID == assetID, "Failed to create base asset with requested ID");

		AssetRegistry& reg = *GetRegistry(type);
		reg.emplace<ACRefsCounters>(assetID);
		reg.emplace<ACDataLocation>(assetID);

		auto& assetMaps = s_Instance->m_AssetMaps[type];

		reg.emplace<ACUUID>(assetID).UUID = assetID;
		assetMaps.ByUUID[(UUID)(uint64_t)assetID] = assetID;

		reg.emplace<ACProxyFilepath>(assetID).Filepath = name;
		assetMaps.ByProxyPath[name] = assetID;

		return assetID;
	}

	void AssetManager::EvaluateAndReload()
	{
		FE_PROFILER_FUNC();

		auto GDI = Renderer::GetActiveGDItype();

		{
			FE_PROFILER_SCOPE("Texture2D");
			// TO DO: doesnt need to be a view, make a group acctually
			auto texturesView = s_Instance->m_Registries[AssetType::Texture2DAsset].view<ACRefsCounters, ACProxyFilepath>();
			for (auto id : texturesView)
			{
				if (BaseAssets::IsBaseAsset(id, AssetType::Texture2DAsset))
					continue;

				auto [acref, acfp] = texturesView.get(id);

				auto textureUser = AssetHandle<Texture2D>(id).Use();
				if (acref.LiveHandles == 0)
				{
					textureUser.UnloadFromGPU();
				}
				else
				{
					if (!textureUser.GetRendererID(GDI))
					{
						TextureLoader::LoadTexture(textureUser);
						textureUser.CreateGDITexture2D(GDI);
					}
				}
				textureUser.UnloadFromCPU();
			}
		}

		{
			FE_PROFILER_SCOPE("Shaders");
			// TO DO: doesnt need to be a view, make a group acctually
			auto shadersView = s_Instance->m_Registries[AssetType::ShaderAsset].view<ACRefsCounters, ACProxyFilepath>();
			for (auto id : shadersView)
			{
				if (BaseAssets::IsBaseAsset(id, AssetType::ShaderAsset))
					continue;

				auto [acref, acfp] = shadersView.get(id);

				auto shaderUser = AssetHandle<Shader>(id).Use();
				if (acref.LiveHandles == 0)
				{
					shaderUser.UnloadFromGPU();
				}
				else
				{
					if (!shaderUser.GetRendererID(GDI))
					{
						ShaderLoader::LoadShader(shaderUser);
						ShaderLoader::CompileShader(GDI, shaderUser);
					}
				}
				shaderUser.UnloadFromCPU();
			}
		}

		{
			FE_PROFILER_SCOPE("Meshes");
			// TO DO: doesnt need to be a view, make a group acctually
			auto meshesView = s_Instance->m_Registries[AssetType::MeshAsset].view<ACRefsCounters, ACProxyFilepath>();
			for (auto id : meshesView)
			{
				if (BaseAssets::IsBaseAsset(id, AssetType::MeshAsset))
					continue;

				auto [acref, acfp] = meshesView.get(id);

				auto meshUser = AssetHandle<Mesh>(id).Use();
				if (acref.LiveHandles == 0)
				{
					meshUser.UnloadFromGPU();
				}
				else
				{
					if (!meshUser.GetBuffers())
					{
						MeshLoader::LoadMesh(meshUser);
						meshUser.SendDataToGPU(GDI, meshUser.GetDataLocation().Data);
					}
				}
				meshUser.UnloadFromCPU();
			}
		}
	}
}