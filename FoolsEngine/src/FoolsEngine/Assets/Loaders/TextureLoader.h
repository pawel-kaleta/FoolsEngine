#pragma once

#include "FoolsEngine\Assets\Loaders\LoaderType.h"
#include "FoolsEngine\Assets\AssetHandle.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Core\Project.h"

#include <string>

namespace fe
{
	class TextureLoader
	{
	public:
		static constexpr LoaderType GetTypeStatic() { return LoaderType::Texture; }
		static constexpr AssetType GetAssetTypeStatic() { return AssetType::Texture2D; }
		static void LoadTexture(const std::filesystem::path& sourceFilePath, const AssetUser<Texture2D>& textureUser);
		static void LoadTexture(const AssetUser<Texture2D>& textureUser)
		{
			auto filepath = Project::GetInstance()->AssetsPath;
			auto source =  textureUser.GetSourceFilepath();
			if (!source)
			{
				FE_CORE_ASSERT(false, "AAAAAAA");
				return;
			}
			else
			{
				filepath /= source->Filepath;
				LoadTexture(filepath, textureUser);
			}
		}
		static void UnloadTexture(void* data);
		static TextureData::Specification InspectTexture(const std::filesystem::path& filePath);
		static bool IsKnownExtension(const std::pmr::string& extension);
		static bool IsKnownAssetType(AssetType assetType);
		static const char* GetExtensionAlias() { return "Texture Source"; }
	};
}