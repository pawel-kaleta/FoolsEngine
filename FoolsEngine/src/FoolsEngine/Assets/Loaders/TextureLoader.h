#pragma once

#include "FoolsEngine/Application/Project.h"

#include "FoolsEngine/Foundation/Memory/String.h"

#include "FoolsEngine/Assets/AssetAccessors.h"
#include "FoolsEngine/Assets/Loaders/LoaderType.h"

#include "FoolsEngine/Renderer/2 - Representation/Texture.h"

#include <filesystem>

namespace fe
{
	class TextureLoader
	{
	public:
		static constexpr LoaderType GetTypeStatic() { return LoaderType::Texture; }
		static constexpr AssetType GetAssetTypeStatic() { return AssetType::Texture2D; }
		static void LoadTexture(const std::filesystem::path& sourceFilePath, const AssetUser& textureUser);
		static void LoadTexture(const AssetUser& textureUser)
		{
			auto filepath = Project::Get()->m_AssetsPath;
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
		static Render::GAPI::Descriptors::TextureSpec InspectTexture(const std::filesystem::path& filePath);
		static bool IsKnownExtension(String extension);
		static bool IsKnownAssetType(AssetType assetType);
		static CString GetExtensionAlias() { return CString("Texture Source",15); }
	};
}