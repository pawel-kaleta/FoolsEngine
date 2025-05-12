#pragma once

#include <filesystem>
#include <memory_resource>

namespace fe
{
	enum AssetType;

	namespace FileHandler
	{
		uint32_t GetSourceAliasAndLoaderIndex(const std::pmr::string& extension, std::pmr::string& outAlias);

		void OpenFile(const std::filesystem::path& filepath);
	};

	class AssetHandleBase;

	namespace AssetImportModal
	{
		void OnImGuiRender();
		void OpenWindow(const std::filesystem::path& filepath, uint32_t loaderIndex, AssetType type, AssetHandleBase* optionalBaseHandle);
	};
}
