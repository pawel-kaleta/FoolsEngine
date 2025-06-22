#pragma once

#include <filesystem>
#include <memory_resource>

#include <FoolsEngine.h>

namespace fe
{
	namespace FileHandler
	{
		uint32_t GetSourceAliasAndLoaderIndex(const std::pmr::string& extension, std::pmr::string& outAlias);
		const AssetTypesRegistry::Item* GetMetaAliasAndRegistryItemPtr(const std::pmr::string& extension, std::pmr::string& outAlias);

		void OpenFile(const std::filesystem::path& filepath, uint32_t loaderIndex = -1);
	};

	class AssetHandleBase;
	struct AssetType;

	namespace AssetImportModal
	{
		void OnImGuiRender();
		void OpenWindow(const std::filesystem::path& filepath, uint32_t loaderIndex, AssetType type = AssetType::None, AssetHandleBase* optionalBaseHandle = nullptr);
	};
}
