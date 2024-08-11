#include "FileHandler.h"

#include <FoolsEngine.h>

#include "AssetImport\AssetImportModal.h"

namespace fe
{
	bool (*IsKnownExtensionPtrs[AssetType::Count])(const std::filesystem::path&) =
	{
		&Scene::IsKnownSourceExtension,
		nullptr, //TextureAsset
		&TextureLoader::IsKnownExtension,
		nullptr, //MeshAsset
		nullptr, //ModelAsset
		nullptr, //ShaderAsset
		nullptr, //MaterialAsset
		nullptr, //MaterialInstanceAsset
		nullptr  //AudioAsset
	};

	std::string(*GetSourceExtensionAliasPtrs[AssetType::Count])() =
	{
		nullptr,
		nullptr,
		&Texture2D::GetSourceExtensionAlias,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	};

	std::string(*GetProxyExtensionPtrs[AssetType::Count])()
	{
		&Scene::GetProxyExtension,
		nullptr,
		& Texture2D::GetProxyExtension,
		nullptr,
		nullptr,
		& Shader::GetProxyExtension,
		nullptr,
		nullptr,
		nullptr
	};

	std::string(*GetProxyExtensionAliasPtrs[AssetType::Count])()
	{
		&Scene::GetProxyExtensionAlias,
			nullptr,
			& Texture2D::GetProxyExtensionAlias,
			nullptr,
			nullptr,
			& Shader::GetProxyExtensionAlias,
			nullptr,
			nullptr,
			nullptr
	};

	bool FileHandler::IsKnownExtension(std::string& extension)
	{
		for (size_t i = 0; i < AssetType::Count; i++)
		{
			if (GetProxyExtensionPtrs[i])
			{
				if ((*GetProxyExtensionPtrs[i])() == extension)
				{
					if (GetProxyExtensionAliasPtrs[i])
					{
						extension = (*GetProxyExtensionAliasPtrs[i])();
						return true;
					}
					else
					{
						FE_CORE_ASSERT(false, "Missing GetProxyExtensionAliasPtr for asset type {0}", i);
					}
				}	
			}

			if (!IsKnownExtensionPtrs[i])
				continue;

			if ((*IsKnownExtensionPtrs[i])(extension))
			{
				if (GetSourceExtensionAliasPtrs[i])
				{
					extension = (*GetSourceExtensionAliasPtrs[i])();
					return true;
				}
				else
				{
					FE_CORE_ASSERT(false, "Missing GetSourceExtensionAliasPtr for asset type {0}", i);
				}
			}
		}

		return false;
	}

	void FileHandler::OpenFile(const std::filesystem::path& filepath)
	{
		if (filepath.empty())
		{
			FE_CORE_ASSERT(false, "Attempt to open file without filepath");
			return;
		}

		for (size_t i = 0; i < AssetType::Count; i++)
		{
			if (!IsKnownExtensionPtrs[i])
				continue;

			if ((*IsKnownExtensionPtrs[i])(filepath.extension()))
			{
				AssetImportModal::OpenWindow(filepath, (AssetType)i, nullptr);
				return;
			}
		}
	}
}