#include "FE_pch.h"

#include "AssetManager.h"

#include "AssetsInclude.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "Loaders\TextureLoader.h"
#include "Loaders\ShaderLoader.h"

namespace fe
{
	AssetManager* AssetManager::s_Instance;

	AssetID AssetManager::CreateAsset(const std::filesystem::path& filePath, AssetType type)
	{
		FE_PROFILER_FUNC();

		AssetID assetID = NewID(type);
		AssetRegistry& reg = *GetRegistry(type);

		reg.emplace<ACRefsCounters>(assetID);
		reg.emplace<ACDataLocation>(assetID);

		UUID uuid = reg.emplace<ACUUID>(assetID).UUID;
		Get().m_AssetMapByUUID[type][uuid] = assetID;

		reg.emplace<ACFilepath>(assetID).Filepath = filePath;
		Get().m_AssetMapByPath[type][filePath] = assetID;

		return assetID;
	}

	AssetID AssetManager::GetAssetWithUUID(UUID uuid, AssetType type)
	{
		FE_PROFILER_FUNC();

		if (0 == uuid)
			return NullAssetID;

		if (Get().m_AssetMapByUUID[type].find(uuid) == Get().m_AssetMapByUUID[type].end())
			return NullAssetID;

		return Get().m_AssetMapByUUID[type].at(uuid);
	}

	AssetID AssetManager::CreateAssetWithUUID(UUID uuid, const std::filesystem::path& filePath, AssetType type)
	{
		AssetID assetID = NewID(type);
		AssetRegistry& reg = *GetRegistry(type);

		reg.emplace<ACRefsCounters>(assetID);
		reg.emplace<ACDataLocation>(assetID);

		reg.emplace<ACUUID>(assetID).UUID = uuid;
		Get().m_AssetMapByUUID[type][uuid] = assetID;

		reg.emplace<ACFilepath>(assetID).Filepath = filePath;
		Get().m_AssetMapByPath[type][filePath] = assetID;

		return assetID;
	}

	AssetID AssetManager::NewBaseAsset(AssetID requestID, const std::string& name, AssetType type)
	{
		AssetID assetID = NewID(type, requestID);

		AssetRegistry& reg = *GetRegistry(type);
		reg.emplace<ACRefsCounters>(assetID);
		reg.emplace<ACDataLocation>(assetID);
		reg.emplace<ACUUID>(assetID);
		reg.emplace<ACFilepath>(assetID).Filepath = name;

		return assetID;
	}

	void AssetManager::EvaluateAndReload()
	{
		FE_PROFILER_FUNC();

		auto& inst = Get();
		auto GDI = Renderer::GetActiveGDItype();

		{
			FE_PROFILER_SCOPE("Texture2D");
			auto texturesView = inst.m_Registries[AssetType::Texture2DAsset].view<ACRefsCounters, ACFilepath>();
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
						TextureLoader::LoadTexture(acfp.Filepath, textureUser);
						textureUser.CreateGDITexture2D(GDI);
					}
				}
				textureUser.UnloadFromCPU();
			}
		}

		{
			FE_PROFILER_SCOPE("Shaders");
			auto shadersView = inst.m_Registries[AssetType::ShaderAsset].view<ACRefsCounters, ACFilepath>();
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
					ShaderLoader::LoadShader(acfp.Filepath, shaderUser);
					ShaderLoader::CompileShader(GDI, shaderUser);
				}
				shaderUser.UnloadFromCPU();
			}
		}
	}
}