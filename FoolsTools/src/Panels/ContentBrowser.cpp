#include "ContentBrowser.h"

#include <FoolsEngine.h>

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

		ImGui::BeginChild("Folders", ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border);
		{
			ImGui::Checkbox("Display Files", &(m_Settings.DisplayFiles));

			for (auto& p : std::filesystem::directory_iterator(m_AssetsPath))
			{
				if (!p.is_directory())
					continue;
				
				RenderFolderNode(p);
			}
			
			if (m_Settings.DisplayFiles)
			{
				for (auto& p : std::filesystem::directory_iterator(m_AssetsPath))
				{
					if (p.is_directory())
						continue;

					RenderFileNode(p);
				}
			}
		}
		ImGui::EndChild();
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, { 0,0 });
		ImGui::SameLine();
		ImGui::PopStyleVar();
		ImGui::BeginChild("Files", {0,0}, ImGuiChildFlags_Border);
		{
			static std::vector<std::filesystem::path> files;
			static std::vector<std::filesystem::path> directories;

			files.clear();
			directories.clear();

			for (auto& p : std::filesystem::directory_iterator(m_CurrentPath))
			{
				if (p.is_directory())
				{
					directories.push_back(p.path().filename());
				}
				else
				{
					files.push_back(p.path().filename());
				}
			}

			static int thumbnailSize = 128;		
			float panelWidth = ImGui::GetContentRegionAvail().x;
			float cellPading = 2 * ImGui::GetStyle().CellPadding.x;
			int columnCount = (int)(panelWidth / (cellPading + thumbnailSize));
			if (columnCount < 1)
				columnCount = 1;

			ImGui::Checkbox("Display Directories", &(m_Settings.DisplayDirectories));
			ImGui::SameLine();
			ImGui::SetNextItemWidth(200);
			ImGui::SliderInt("Thumbnail Size", &thumbnailSize, 16, 256);

			if (ImGui::BeginTable("table", columnCount))
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.29f, 0.48f, 0.3f));

				auto gdi = Renderer::GetActiveGDItype();
				ImVec2 thumbnailSizeIm((float)thumbnailSize, (float)thumbnailSize);
				if (m_Settings.DisplayDirectories)
				{
					auto textureUser = m_Icons.Folder.Use();

					if (m_CurrentPath.compare(m_AssetsPath))
					{
						ImGui::TableNextColumn();
						ImGui::ImageButton("UP", (ImTextureID)textureUser.GetRendererID(gdi), thumbnailSizeIm, { 0,1 }, { 1,0 });

						if (ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)
							&&
							ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
						{
							m_CurrentPath = m_CurrentPath.parent_path();
						}

						ImGui::TextWrapped("..");
					}

					for (auto& folder : directories)
					{
						ImGui::TableNextColumn();
						ImGui::ImageButton(folder.string().c_str(), (ImTextureID)textureUser.GetRendererID(gdi), thumbnailSizeIm, { 0,1 }, { 1,0 });

						if (ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)
							&&
							ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
						{
							m_CurrentPath /= folder;
						}

						ImGui::TextWrapped(folder.string().c_str());
					}
				}

				auto textureUser = m_Icons.File.Use();
				for (auto& file : files)
				{
					ImGui::TableNextColumn();
					ImGui::ImageButton(file.string().c_str(), (ImTextureID)textureUser.GetRendererID(gdi), thumbnailSizeIm, { 0,1 }, { 1,0 });

					ImGui::TextWrapped(file.string().c_str());

				}

				ImGui::PopStyleColor(2);
				ImGui::EndTable();
			}

			
		}
		ImGui::EndChild();

		ImGui::End();
	}

	void ContentBrowser::RenderFolderNode(const std::filesystem::directory_entry& dir)
	{
		if (!dir.path().lexically_relative(m_CurrentPath).compare(".."))
		{
			ImGui::SetNextItemOpen(true);
		}
		else if (dir.path().compare(m_CurrentPath) && m_Settings.DisplayDirectories)
		{
			ImGui::SetNextItemOpen(false);
		}

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