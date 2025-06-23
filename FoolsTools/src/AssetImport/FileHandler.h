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
		void OpenWindow(const std::filesystem::path& filepath, const LoadersRegistry::Item* loaderItemPtr, AssetType type = AssetType::None, AssetHandleBase* optionalBaseHandle = nullptr);
	};
}
