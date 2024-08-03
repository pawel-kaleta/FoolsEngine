#include "ContentBrowser.h"

#include <FoolsEngine.h>

#include "AssetImport\TextureImport.h"

namespace fe
{
	ContentBrowser::ContentBrowser()
	{
		FE_PROFILER_FUNC();

		m_Icons.File = EditorAssetHandle<Texture2D>(EditorAssetManager::NewAsset<Texture2D>());
		m_Icons.Folder = EditorAssetHandle<Texture2D>(EditorAssetManager::NewAsset<Texture2D>());

		auto fileUser = m_Icons.File.Use();
		auto folderUser = m_Icons.Folder.Use();

		TextureLoader::LoadTexture("resources/File.png", fileUser);
		TextureLoader::LoadTexture("resources/Folder.png", folderUser);
		
		auto GDI = Renderer::GetActiveGDItype();
		fileUser.CreateGDITexture2D(GDI);
		folderUser.CreateGDITexture2D(GDI);
	}

	void ContentBrowser::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Content Browser");
		ImGui::PopStyleVar();

		RenderFolderHierarchy();
		
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, { 0,0 });
		ImGui::SameLine();
		ImGui::PopStyleVar();

		RenderFolderContent();

		ImGui::End();
	}

	void ContentBrowser::RenderFolderHierarchy()
	{
		if (!ImGui::BeginChild("Folders", ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border))
			return;
		
		ImGui::Checkbox("Display Files", &(m_Settings.DisplayFiles));

		RenderFolderNode(std::filesystem::directory_entry(m_AssetsPath));
		ImGui::EndChild();
	}

	void ContentBrowser::RenderFolderContent()
	{
		if (!ImGui::BeginChild("Files settings", { 0,0 }, ImGuiChildFlags_Border))
			return;

		ReadFolder();

		ImGui::Checkbox("Display Directories", &(m_Settings.DisplayDirectories));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(120);
		ImGui::SliderInt("Thumbnail Size", &(m_Settings.ThumbnailSize), 16, 256);


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.29f, 0.48f, 0.3f));

		if (m_Settings.DisplayDirectories)
			RenderFolders();

		RenderFiles();

		ImGui::PopStyleColor(2);
		
		ImGui::EndChild();
	}

	void ContentBrowser::RenderFolders()
	{
		auto& tnSize = m_Settings.ThumbnailSize;
		ImVec2 thumbnailSizeIm((float)tnSize, (float)tnSize);
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		ImGuiStyle& style = ImGui::GetStyle();
		auto gdi = Renderer::GetActiveGDItype();

		auto textureUser = m_Icons.Folder.Use();
		if (m_CurrentPath.compare(m_AssetsPath))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_CellPadding, { 0.f,0.f });
			ImGui::BeginTable("UP", 1, 0, { (float)tnSize + 9.f, (float)tnSize * 2 });
			ImGui::TableNextColumn();
			ImGui::ImageButton("UP2", (ImTextureID)(int64_t)textureUser.GetRendererID(gdi), thumbnailSizeIm, { 0,1 }, { 1,0 });
			ImGui::PopStyleVar();

			if (ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)
				&&
				ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
			{
				m_CurrentPath = m_CurrentPath.parent_path();
			}

			ImGui::TableNextColumn();
			ImGui::Text("..");
			//ImGui::TableNextColumn();
			ImGui::EndTable();

			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + style.ItemSpacing.x + tnSize + 9; // Expected position if next element was on same line
			if (next_button_x2 < window_visible_x2)
				ImGui::SameLine();
		}

		for (auto& folder : m_Directories)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_CellPadding, { 0.f,0.f });
			ImGui::BeginTable(folder.string().c_str(), 1, 0, { (float)tnSize + 9.f, (float)tnSize * 2 });
			ImGui::TableNextColumn();
			ImGui::ImageButton(folder.string().c_str(), (ImTextureID)(int64_t)textureUser.GetRendererID(gdi), thumbnailSizeIm, { 0,1 }, { 1,0 });
			ImGui::PopStyleVar();

			if (ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)
				&&
				ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
			{
				m_CurrentPath /= folder;
			}

			ImGui::TableNextColumn();
			ImGui::Text(folder.string().c_str());
			//ImGui::TableNextColumn();
			ImGui::EndTable();

			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + style.ItemSpacing.x + tnSize + 9; // Expected position if next button was on same line
			if (next_button_x2 < window_visible_x2)
				ImGui::SameLine();
		}
	}

	void ContentBrowser::RenderFiles()
	{
		auto& tnSize = m_Settings.ThumbnailSize;
		ImVec2 thumbnailSizeIm((float)tnSize, (float)tnSize);
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		ImGuiStyle& style = ImGui::GetStyle();
		auto gdi = Renderer::GetActiveGDItype();

		std::filesystem::path fileOpenAttempt;

		for (auto& file : m_Files)
		{
			std::string extension = file.extension().string();
			bool recognizedAsset = false;
			bool recognizedAssetSource = false;
			if (extension == ".fescene") { extension = "Scene"; recognizedAsset = true; }
			else if (extension == ".glsl") { extension = "Shader Source"; recognizedAssetSource = true; }
			else if (extension == ".fetex2d") { extension = "Texture2D"; recognizedAsset = true; }
			else if (TextureLoader::IsKnownExtension(extension)) { extension = "Texture Source"; recognizedAssetSource = true; }

			if (recognizedAssetSource || recognizedAsset)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_CellPadding, { 0.f,0.f });
				ImGui::BeginTable(file.string().c_str(), 1, ImGuiTableFlags_BordersOuter, { (float)tnSize + 9.f, 0 });
				ImGui::PopStyleVar();
				ImGui::TableNextColumn();

				// thumbnail in the future
				ImGui::ImageButton(file.string().c_str(), (ImTextureID)(int64_t)m_Icons.File.Use().GetRendererID(gdi), thumbnailSizeIm, { 0,1 }, { 1,0 });
				{
					if (ImGui::IsItemClicked(0) && ImGui::IsMouseDoubleClicked(0))
					{
						fileOpenAttempt = file;
					}

					ImGui::BeginTable(file.string().c_str(), 1, ImGuiTableFlags_PadOuterX);
					ImGui::TableNextRow(ImGuiTableRowFlags_None, 25.f);
					ImGui::TableNextColumn();
					ImGui::Text(file.stem().string().c_str());
					ImGui::TableNextColumn();

					ImGui::TextWrapped(extension.c_str());
					ImGui::EndTable();
				}

				ImGui::EndTable();
			}
			else
			{
				ImGui::BeginTable(file.string().c_str(), 1, 0, { (float)tnSize + 9.f, 0 });
				ImGui::TableNextColumn();
				ImGui::ImageButton(file.string().c_str(), (ImTextureID)(int64_t)m_Icons.File.Use().GetRendererID(gdi), thumbnailSizeIm, { 0,1 }, { 1,0 });
				ImGui::TableNextColumn();
				ImGui::TextWrapped(file.filename().string().c_str());
				ImGui::EndTable();
			}

			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + style.ItemSpacing.x + tnSize + 9; // Expected position if next button was on same line
			if (next_button_x2 < window_visible_x2)
				ImGui::SameLine();
		}

		if (!fileOpenAttempt.empty())
		{
			if (TextureLoader::IsKnownExtension(fileOpenAttempt.extension().string()))
			{
				TextureImport::OpenWindow(m_CurrentPath / fileOpenAttempt);
			}	
		}

		TextureImport::RenderWindow();
		
	}

	void ContentBrowser::ReadFolder()
	{
		m_Files.clear();
		m_Directories.clear();

		for (auto& p : std::filesystem::directory_iterator(m_CurrentPath))
		{
			if (p.is_directory())
			{
				m_Directories.push_back(p.path().filename());
			}
			else
			{
				m_Files.push_back(p.path().filename());
			}
		}
	}

	void ContentBrowser::RenderFolderNode(const std::filesystem::directory_entry& dir)
	{
		if (dir.path().compare(m_CurrentPath) != 0 && m_Settings.DisplayDirectories)
			ImGui::SetNextItemOpen(false);
		if (dir.path().lexically_relative(m_CurrentPath).compare("..")==0)
			ImGui::SetNextItemOpen(true);
		if (dir.path().compare(m_CurrentPath) == 0)
			ImGui::SetNextItemOpen(true);
		if (dir.path().compare(m_AssetsPath) == 0)
			ImGui::SetNextItemOpen(true);


		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		bool selected = (m_CurrentPath == dir);
		flags |= selected ? ImGuiTreeNodeFlags_Selected : 0;

		bool open = ImGui::TreeNodeEx(dir.path().string().c_str(), flags, dir.path().filename().string().c_str());

		if (ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen())
			m_CurrentPath = dir;

		if (!open)
			return;
		
		for (auto& p : std::filesystem::directory_iterator(dir))
		{
			if (!p.is_directory())
				continue;

			RenderFolderNode(p);
		}

		if (m_Settings.DisplayFiles)
		{
			for (auto& p : std::filesystem::directory_iterator(dir))
			{
				if (p.is_directory())
					continue;

				RenderFileNode(p);
			}
		}

		ImGui::TreePop();
	}

	void ContentBrowser::RenderFileNode(const std::filesystem::directory_entry& file)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;// | ImGuiTreeNodeFlags_Bullet;

		ImGui::TreeNodeEx(file.path().c_str(), flags, file.path().filename().string().c_str());
	}
}