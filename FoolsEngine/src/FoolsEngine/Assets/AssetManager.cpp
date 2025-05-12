#include "FE_pch.h"
#include "AssetManager.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "Loaders\TextureLoader.h"
#include "Loaders\ShaderLoader.h"
#include "Loaders\GeometryLoader.h"

#include "Serializers\YAML.h"

namespace fe
{
	AssetManager* AssetManager::s_Instance;

	AssetID AssetManager::GetAssetFromFilepath(const std::filesystem::path& filepath, AssetType type)
	{
		auto& map = s_Instance->m_Data[type].MapByFilepath;
		auto search_result = map.find(filepath);
		if (search_result != map.end())
			return search_result->second;
		return NullAssetID;
	}

	AssetID AssetManager::GetAssetWithUUID(UUID uuid, AssetType type)
	{
		FE_PROFILER_FUNC();
	
		auto& assetMap = s_Instance->m_Data[type].MapByUUID;
		auto asset = assetMap.find(uuid);
	
		if (asset == assetMap.end())
			return NullAssetID;
	
		return asset->second;
	}

	void AssetManager::EvaluateAndReload()
	{
		FE_PROFILER_FUNC();

		auto GDI = Renderer::GetActiveGDItype();

		{
			FE_PROFILER_SCOPE("Texture2D");
			// TO DO: doesnt need to be a view, make a group acctually
			auto texturesView = s_Instance->m_Data[AssetType::Texture2DAsset].Registry.view<ACRefsCounters, ACFilepath>();
			for (auto id : texturesView)
			{
				auto [acref, acfp] = texturesView.get(id);

				auto textureUser = AssetHandle<Texture2D>(id).Use();
				if (acref.LiveHandles == 0)
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
			}
		}

		{
			FE_PROFILER_SCOPE("Shaders");
			// TO DO: doesnt need to be a view, make a group acctually
			auto shadersView = s_Instance->m_Data[AssetType::ShaderAsset].Registry.view<ACRefsCounters, ACFilepath>();
			for (auto id : shadersView)
			{
				auto [acref, acfp] = shadersView.get(id);

				auto shaderUser = AssetHandle<Shader>(id).Use();
				if (acref.LiveHandles == 0)
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
				//shaderUser.UnloadFromCPU();
			}
		}

		{
			FE_PROFILER_SCOPE("Meshes");
			// TO DO: doesnt need to be a view, make a group acctually
			auto meshesView = s_Instance->m_Data[AssetType::MeshAsset].Registry.view<ACRefsCounters, ACFilepath>();
			for (auto id : meshesView)
			{
				auto [acref, acfp] = meshesView.get(id);

				auto meshUser = AssetHandle<Mesh>(id).Use();
				if (acref.LiveHandles == 0)
				{
					meshUser.Release();
				}
				else
				{
					if (!meshUser.GetBuffers())
					{
						GeometryLoader::LoadMesh(meshUser);
						meshUser.SendDataToGPU(GDI, meshUser.GetDataLocation().Data);
						meshUser.UnloadFromCPU();
					}
				}
			}
		}
	}
}