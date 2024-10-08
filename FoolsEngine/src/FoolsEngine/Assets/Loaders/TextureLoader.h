#pragma once

#include "FoolsEngine\Assets\AssetHandle.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"

namespace fe
{
	class TextureLoader
	{
	public:
		static void LoadTexture(const std::filesystem::path& sourceFilePath, AssetUser<Texture2D>& textureUser);
		static void LoadTexture(AssetUser<Texture2D>& textureUser)
		{
			LoadTexture(textureUser.GetSourceFilepath(), textureUser);
		}
		static void UnloadTexture(AssetUser<Texture2D>& textureUser);
		static TextureData::Specification InspectTexture(const std::filesystem::path& filePath);
		static bool IsKnownExtension(const std::filesystem::path& extension);
	};
}