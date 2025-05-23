#pragma once

#include <filesystem>
#include <memory_resource>

#include "EditorAssetHandle.h"

namespace fe
{
	class Scratchpad;

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
			uint32_t ThumbnailSize = 100;

			bool DisplayFiles		= false;
			bool DisplayDirectories	= true;
		} m_Settings;

		struct Icons
		{
			EditorAssetHandle<Texture2D> File;
			EditorAssetHandle<Texture2D> Folder;
		} m_Icons;

		Scratchpad* m_SP;
		
		using PmrVecString = std::pmr::vector<std::pmr::string>;
		PmrVecString* m_Files;
		PmrVecString* m_Directories;

		void RenderFolderHierarchy();
		void RenderFolderContent();

		void RenderFolders();
		void RenderFiles();

		void ReadFolder();

		void RenderFolderNode(const std::filesystem::directory_entry& dir);
		void RenderFileNode(const std::filesystem::directory_entry& file);
	};
}