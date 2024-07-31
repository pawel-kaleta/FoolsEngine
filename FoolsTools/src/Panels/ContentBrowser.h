#pragma once

#include <filesystem>

#include "EditorAssetManager.h"

namespace fe
{
	class ContentBrowser
	{
	public:
		ContentBrowser();
		void OnImGuiRender();

	private:

		std::filesystem::path m_AssetsPath = "assets"; //TO DO: project relative
		std::filesystem::path m_CurrentPath = "assets";

		struct Settings
		{
			bool DisplayFiles		= false;
			bool DisplayDirectories	= true;
		} m_Settings;

		struct Icons
		{
			EditorAssetHandle<Texture2D> File;
			EditorAssetHandle<Texture2D> Folder;
		} m_Icons;

		void RenderFolderNode(const std::filesystem::directory_entry& dir);
		void RenderFileNode(const std::filesystem::directory_entry& file);
	};
}