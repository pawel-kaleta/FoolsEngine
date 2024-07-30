#include "FE_pch.h"

#include "AssetManager.h"

#include "AssetsInclude.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "Loaders\TextureLoader.h"
#include "Loaders\ShaderLoader.h"

namespace fe
{
	AssetManager* AssetManager::s_Instance;

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

	AssetID AssetManager::CreateAssetWithUUID(AssetType type, UUID uuid)
	{
		auto& inst = Get();
		auto id = NewAsset(type);

		inst.m_Registries[type].get<ACUUID>(id).UUID = uuid;
		inst.m_AssetMapByUUID[type][uuid] = id;

		return id;
	}
}