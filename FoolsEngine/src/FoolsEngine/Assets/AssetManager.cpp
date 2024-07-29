#include "FE_pch.h"

#include "AssetManager.h"

#include "AssetsInclude.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "Loaders\TextureLoader.h"
#include "Loaders\ShaderLoader.h"

namespace fe
{
	void AssetManager::ClearRegistries()
	{
		auto& inst = Get();
		for (size_t i = 0; i < (size_t)AssetType::Count; i++)
		{
			AssetType type = (AssetType)i;
			auto& reg = inst.m_Registries[type];
			
			auto view = reg.view<ACDataLocation>();
			for (auto id : view)
			{
				auto [dl] = view.get(id);
				free(dl.Data);
				dl.Data = nullptr;
			}

			reg.clear();
			reg = AssetRegistry();

			inst.m_AssetMapByUUID[type].clear();
		}


		Renderer::CreateBaseAssets();
		Renderer::UploadBaseAssetsToGPU(Renderer::GetActiveGDItype());
	}

	void AssetManager::EvaluateAndReload()
	{
		auto& inst = Get();
		auto GDI = Renderer::GetActiveGDItype();

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

	AssetID AssetManager::CreateAssetWithUUID(AssetType type, UUID uuid)
	{
		auto& inst = Get();
		auto id = NewAsset(type);

		inst.m_Registries[type].get<ACUUID>(id).UUID = uuid;
		inst.m_AssetMapByUUID[type][uuid] = id;

		return id;
	}
}