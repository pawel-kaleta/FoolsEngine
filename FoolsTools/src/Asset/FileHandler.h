#pragma once

#include <filesystem>
#include <memory_resource>

#include <FoolsEngine.h>

namespace fe
{
	namespace FileHandler
	{
		void OpenFile(const std::filesystem::path& filepath, const LoadersRegistry::Item* loaderItemPtr);
	};

	class AssetHandleBase;
	struct AssetType;

	namespace AssetImportModal
	{
		void Init();
		void OnImGuiRender();
		void OpenWindow(const std::filesystem::path& filepathToImport, const LoadersRegistry::Item* loaderItemPtr);

		template <typename tnAsset>
		std::filesystem::path GetDefaultFilepathAndFilterForImport(const std::filesystem::path& originalPath, std::pmr::string& outFilter)
		{
			Scratchpad sp;
			std::pmr::string alias(&sp);
			std::pmr::string extension(&sp);

			alias = AssetType(tnAsset::GetTypeStatic()).ToConstCharPtr();
			extension = tnAsset::GetMetaFileExtension();

			outFilter = alias + " (" + extension + ")" + std::pmr::string(1, '\0', &sp) + "*" + extension + std::pmr::string(1, '\0', &sp);

			std::filesystem::path defaultFilepath = originalPath;
			defaultFilepath = std::filesystem::absolute(defaultFilepath.replace_extension(std::filesystem::path(extension)));

			return defaultFilepath;
		}
	};
}
