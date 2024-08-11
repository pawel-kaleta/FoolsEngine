#pragma once

#include <filesystem>

namespace fe
{
	class std::filesystem::path;
	enum AssetType;
	class AssetHandleBase;

	namespace AssetImportModal
	{
		void OnImGuiRender();
		void OpenWindow(const std::filesystem::path& filepath, AssetType type, AssetHandleBase* optionalBaseHandle);
	};
}