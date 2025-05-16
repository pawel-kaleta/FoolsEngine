#include "FE_pch.h"
#include "AssetManager.h"

#include "AssetHandle.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "Loaders\TextureLoader.h"
#include "Loaders\ShaderLoader.h"
#include "Loaders\GeometryLoader.h"

#include "Serializers\YAML.h"

namespace fe
{
	AssetManager* AssetManager::s_Instance;

	AssetID AssetManager::GetAssetFromFilepath(const std::filesystem::path& filepath)
	{
		auto& map = s_Instance->m_MapByFilepath;
		auto search_result = map.find(filepath);
		if (search_result != map.end())
			return search_result->second;
		return NullAssetID;
	}

	AssetID AssetManager::GetAssetWithUUID(UUID uuid)
	{
		FE_PROFILER_FUNC();
	
		auto& assetMap = s_Instance->m_MapByUUID;
		auto asset = assetMap.find(uuid);
	
		if (asset == assetMap.end())
			return NullAssetID;
	
		return asset->second;
	}

	void AssetManager::EvaluateAndReload()
	{
		FE_PROFILER_FUNC();

		auto GDI = Renderer::GetActiveGDItype();
		auto& reg = s_Instance->m_Registry;

		auto view = reg.view<ACRefsCounters>();

		for (auto id : view)
		{
			auto& ref_counters = view.get<ACRefsCounters>(id);
			auto type = reg.get<ACAssetType>(id).Type;

			switch (type)
			{
			case AssetType::Texture2DAsset:
			{
				auto textureUser = AssetHandle<Texture2D>(id).Use();
				if (ref_counters.LiveHandles == 0)
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
				break;
			}
			case AssetType::ShaderAsset:
			{
				auto shaderUser = AssetHandle<Shader>(id).Use();
				if (ref_counters.LiveHandles == 0)
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
				break;
			}
			case AssetType::MeshAsset:
			{
				auto meshUser = AssetHandle<Mesh>(id).Use();
				if (ref_counters.LiveHandles == 0)
				{
					meshUser.Release();
				}
				else
				{
					if (!meshUser.GetBuffers())
					{
						GeometryLoader::LoadMesh(meshUser);
						meshUser.SendDataToGPU(GDI);
						meshUser.UnloadFromCPU();
					}
				}
			}
			}
		}
	}
}