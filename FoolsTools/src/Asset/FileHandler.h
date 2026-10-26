#pragma once

#include <filesystem>
#include <memory_resource>

#include <FoolsEngine.h>

namespace fe
{
	namespace FileHandler
	{
		void OpenFile(const std::filesystem::path& filepath, const TypeRegs::Loaders::Item* loaderItemPtr);
	};

	class AssetHandleBase;
	struct AssetType;

	namespace AssetImportModal
	{
		void Init();
		void OnImGuiRender();
		void OpenWindow(const std::filesystem::path& filepathToImport, const TypeRegs::Loaders::Item* loaderItemPtr);

		template <typename tnAsset>
		std::filesystem::path GetDefaultFilepathAndFilterForImport(const std::filesystem::path& originalPath, std::pmr::string& outFilter)
		{
			Pile p;
			STD_PMR_Allocator pmr(&p);

			std::pmr::string alias(&pmr);
			std::pmr::string extension(&pmr);

			alias = AssetType(tnAsset::GetTypeStatic()).ToCString().Data;
			extension = tnAsset::GetMetaFileExtension();

			outFilter = alias + " (" + extension + ")" + std::pmr::string(1, '\0', &pmr) + "*" + extension + std::pmr::string(1, '\0', &pmr);

			std::filesystem::path default_filepath = originalPath;
			default_filepath = std::filesystem::absolute(default_filepath.replace_extension(std::filesystem::path(extension)));

			return default_filepath;
		}
	};
}
