#pragma once

#include "FoolsEngine\Assets\AssetHandle.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"

#include <string>

namespace fe
{
	class TextureLoader
	{
	public:
		static void LoadTexture(const std::filesystem::path& sourceFilePath, const AssetUser<Texture2D>& textureUser);
		static void LoadTexture(const AssetUser<Texture2D>& textureUser)
		{
			LoadTexture(textureUser.GetFilepath(), textureUser);
		}
		static void UnloadTexture(void* data);
		static TextureData::Specification InspectTexture(const std::filesystem::path& filePath);
		static bool IsKnownExtension(const std::pmr::string& extension);
		static const char* GetExtensionAlias() { return "Texture Source"; }
	};
}