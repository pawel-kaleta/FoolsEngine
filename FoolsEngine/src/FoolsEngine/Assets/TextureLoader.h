#pragma once

#include <filesystem>
//#include "AssetHandle.h"
#include "FoolsEngine/Renderer/2 - GDIAbstraction/Texture.h"

namespace fe
{
	class TextureLoader
	{
	public:
		static AssetHandle<Texture> LoadTexture(const std::filesystem::path& filePath)
		{
			auto textureSignaturePtr = AssetSignatureRegistry::GenerateNew();

			auto texturePtr = Texture2D::Create(filePath, TextureData::Usage{0}, textureSignaturePtr);
			
			AssetLibrary::Add((Asset*)texturePtr);

			return AssetLibrary::GetAssetHandle<Texture>(textureSignaturePtr->ID);
		}

		static Texture* LoadAnonimousTexture(const std::filesystem::path& filePath)
		{
			return Texture2D::Create(filePath, TextureData::Usage{0}, nullptr);
		}
	private:

	};
}