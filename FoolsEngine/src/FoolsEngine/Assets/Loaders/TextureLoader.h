#pragma once

#include <filesystem>
#include "FoolsEngine\Assets\AssetHandle.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"

#include <stb_image.h>

namespace fe
{
	class TextureLoader
	{
	public:
		static void LoadTexture(const std::filesystem::path& filePath, AssetUser<Texture2D>& newTexture);
		static void UnloadTexture(AssetUser<Texture2D>& textureUser);
	};
}