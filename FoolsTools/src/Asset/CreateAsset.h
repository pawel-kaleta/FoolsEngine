#pragma once

#include <FoolsEngine.h>

#include <filesystem>

namespace fe
{
	namespace CreateAsset
	{
		void Init();
		void OnImGuiRender();
		void OpenWindow(const std::filesystem::path& filepath, AssetType assetType);

		void CreateMaterial(const std::filesystem::path& filepath);
	};
}
