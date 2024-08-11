#pragma once

#include <filesystem>

namespace fe
{
	class FileHandler
	{
	public:
		static bool IsKnownExtension(std::string& extension);
		static void OpenFile(const std::filesystem::path& filepath);
	private:
		//static bool (*IsKnownExtensionPtrs[AssetType::Count])(const std::filesystem::path&);
		//static std::string(*GetSourceExtensionAliasPtrs[AssetType::Count])();
		//static std::string(*GetProxyExtensionPtrs[AssetType::Count])();
		//static std::string(*GetProxyExtensionAliasPtrs[AssetType::Count])();
	};
}