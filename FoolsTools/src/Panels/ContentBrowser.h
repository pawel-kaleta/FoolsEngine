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
		friend class EditorLayer;

		std::filesystem::path m_AssetsPath;
		std::filesystem::path m_CurrentPath;

		struct Settings
		{
			uint32_t ThumbnailSize = 100;

			bool DisplayFiles		= false;
			bool DisplayDirectories	= true;
		} m_Settings;

		struct
		{
			EditorAssetHandle<Texture2D> File;
			EditorAssetHandle<Texture2D> Folder;
			void* FileID;
			void* FolderID;
		} m_Icons;

		void RenderFiles(std::pmr::vector<std::pmr::string>* file_names);
		void RenderFolders(Scratchpad* sp, std::pmr::vector<std::pmr::string>* file_names);
		void RenderFiles();
		void RenderFile(const std::pmr::string& stem, const std::pmr::string& extension);
		void RenderUPFolder();
		void RenderFolderNode(const std::filesystem::directory_entry& dir);
	};
}