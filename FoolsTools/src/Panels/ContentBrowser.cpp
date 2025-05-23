#include "ContentBrowser.h"

#include <FoolsEngine.h>

#include "AssetImport\FileHandler.h"

namespace fe
{
	ContentBrowser::ContentBrowser()
	{
		FE_PROFILER_FUNC();

		m_Icons.File = EditorAssetHandle<Texture2D>(AssetManager::NewAsset<Texture2D>());
		m_Icons.Folder = EditorAssetHandle<Texture2D>(AssetManager::NewAsset<Texture2D>());

		TextureLoader::LoadTexture("resources/File.png", m_Icons.File);
		TextureLoader::LoadTexture("resources/Folder.png", m_Icons.Folder);
		
		auto GDI = Renderer::GetActiveGDItype();
		m_Icons.File.CreateGDITexture2D(GDI);
		m_Icons.Folder.CreateGDITexture2D(GDI);
	}

	void ContentBrowser::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

		Scratchpad sp;
		m_SP = &sp;
		m_Files = sp.NewObject<PmrVecString>();
		m_Directories = sp.NewObject<PmrVecString>();

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
		FE_PROFILER_FUNC();

		if (!ImGui::BeginChild("Folders", ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border))
			return;
		
		ImGui::Checkbox("Display Files", &(m_Settings.DisplayFiles));

		RenderFolderNode(std::filesystem::directory_entry(m_AssetsPath));
		ImGui::EndChild();
	}

	void ContentBrowser::RenderFolderContent()
	{
		FE_PROFILER_FUNC();

		if (!ImGui::BeginChild("Files settings", { 0,0 }, ImGuiChildFlags_Border))
			return;

		ReadFolder();

		ImGui::Checkbox("Display Directories", &(m_Settings.DisplayDirectories));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(120);
		ImGui::SliderInt("Thumbnail Size", (int*) & (m_Settings.ThumbnailSize), 16, 256);

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
		FE_PROFILER_FUNC();

		auto& tnSize = m_Settings.ThumbnailSize;
		ImVec2 thumbnailSizeIm((float)tnSize, (float)tnSize);
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		ImGuiStyle& style = ImGui::GetStyle();
		auto gdi = Renderer::GetActiveGDItype();

		ImTextureID texture_id = (ImTextureID)(int64_t)m_Icons.Folder.GetRendererID(gdi);

		if (m_CurrentPath.compare(m_AssetsPath))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_CellPadding, { 0.f,0.f });
			ImGui::BeginTable("UP", 1, 0, { (float)(tnSize + 9), (float)(tnSize * 2) });
			ImGui::TableNextColumn();
			ImGui::ImageButton("UP2", texture_id, thumbnailSizeIm, { 0,1 }, { 1,0 });
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

		for (auto& folder : *m_Directories)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_CellPadding, { 0.f,0.f });
			ImGui::BeginTable(folder.c_str(), 1, 0, { (float)(tnSize + 9), (float)(tnSize * 2) });
			ImGui::TableNextColumn();
			ImGui::ImageButton(folder.c_str(), texture_id, thumbnailSizeIm, { 0,1 }, { 1,0 });
			ImGui::PopStyleVar();

			if (ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)
				&&
				ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
			{
				m_CurrentPath /= folder;
			}

			ImGui::TableNextColumn();
			ImGui::Text(folder.c_str());
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
		FE_PROFILER_FUNC();

		auto& tnSize = m_Settings.ThumbnailSize;
		ImVec2 thumbnailSizeIm((float)tnSize, (float)tnSize);
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		ImGuiStyle& style = ImGui::GetStyle();
		auto gdi = Renderer::GetActiveGDItype();

		ImTextureID file_icon_id = (ImTextureID)m_Icons.File.GetRendererID(gdi);

		for (auto& file : *m_Files)
		{
			auto dot_pos = file.rfind(".");
			auto extension = file.substr(dot_pos, file.length() - dot_pos);

			std::pmr::string alias(m_SP);

			bool is_asset_source = FileHandler::GetSourceAliasAndLoaderIndex(extension, alias) != -1;

			//TO DO: check for meta file??
			if (is_asset_source)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_CellPadding, { 0.f,0.f });
				ImGui::BeginTable(file.c_str(), 1, ImGuiTableFlags_BordersOuter, { (float)(tnSize + 9), 0 });
				ImGui::PopStyleVar();
				ImGui::TableNextColumn();

				// thumbnail in the future
				
				ImGui::ImageButton(file.c_str(), file_icon_id, thumbnailSizeIm, { 0,1 }, { 1,0 });
				{
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						// Set payload to carry the asset filepath of our item (could be anything)
						static std::filesystem::path payload;
						payload = m_CurrentPath / file;
						ImGui::SetDragDropPayload("AssetPath", &payload, sizeof(payload));

						// Display preview (could be anything, e.g. when dragging an image we could decide to display
						// the filename and a small preview of the image, etc.)
						ImGui::ImageButton(file.c_str(), file_icon_id, { 32,32 }, { 0,1 }, { 1,0 });
						ImGui::Text(file.c_str());

						ImGui::EndDragDropSource();
					}


					if (ImGui::IsItemClicked(0) && ImGui::IsMouseDoubleClicked(0))
					{
						FileHandler::OpenFile(m_CurrentPath / file);
					}

					ImGui::BeginTable(file.c_str(), 1, ImGuiTableFlags_PadOuterX);
					ImGui::TableNextRow(ImGuiTableRowFlags_None, 25.f);
					ImGui::TableNextColumn();
					ImGui::TextWrapped(file.c_str());
					ImGui::TableNextColumn();

					ImGui::TextWrapped(alias.c_str());
					ImGui::EndTable();
				}

				ImGui::EndTable();
			}
			else
			{
				ImGui::BeginTable(file.c_str(), 1, 0, { (float)(tnSize + 9), 0 });
				ImGui::TableNextColumn();
				ImGui::ImageButton(file.c_str(), file_icon_id, thumbnailSizeIm, { 0,1 }, { 1,0 });
				ImGui::TableNextColumn();
				ImGui::TextWrapped(file.c_str());
				ImGui::EndTable();
			}

			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + style.ItemSpacing.x + tnSize + 9; // Expected position if next button was on same line
			if (next_button_x2 < window_visible_x2)
				ImGui::SameLine();
		}
	}

	void ContentBrowser::ReadFolder()
	{
		FE_PROFILER_FUNC();

		for (auto& p : std::filesystem::directory_iterator(m_CurrentPath))
		{
			//auto y = p.path().string<PMR_STRING_TEMPLATE_PARAMS>(m_MBR);
			auto y = p.path().string<PMR_STRING_TEMPLATE_PARAMS>(m_SP);

			auto lastSlash = y.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto count = y.size() - lastSlash;
			auto z = y.substr(lastSlash, count);

			if (p.is_directory())
			{
				m_Directories->push_back(std::move(z));
			}
			else
			{
				m_Files->push_back(std::move(z));
			}
		}
	}

	void ContentBrowser::RenderFolderNode(const std::filesystem::directory_entry& dir)
	{
		FE_PROFILER_FUNC();

		if (dir.path().compare(m_CurrentPath) != 0 && m_Settings.DisplayDirectories)
			ImGui::SetNextItemOpen(false);
		if (dir.path().lexically_relative(m_CurrentPath).compare("..")==0) // this is expensive
			ImGui::SetNextItemOpen(true);
		if (dir.path().compare(m_CurrentPath) == 0)
			ImGui::SetNextItemOpen(true);
		if (dir.path().compare(m_AssetsPath) == 0)
			ImGui::SetNextItemOpen(true);


		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		bool selected = (m_CurrentPath == dir);
		flags |= selected ? ImGuiTreeNodeFlags_Selected : 0;

		//auto node_name = dir.path().filename().string<PMR_STRING_TEMPLATE_PARAMS>(m_MBR);
		auto node_name = dir.path().filename().string<PMR_STRING_TEMPLATE_PARAMS>(m_SP);
		bool open = ImGui::TreeNodeEx(node_name.c_str(), flags, node_name.c_str());

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
		FE_PROFILER_FUNC();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;// | ImGuiTreeNodeFlags_Bullet;

		//auto node_name = file.path().filename().string<PMR_STRING_TEMPLATE_PARAMS>(m_MBR);
		auto node_name = file.path().filename().string<PMR_STRING_TEMPLATE_PARAMS>(m_SP);
		ImGui::TreeNodeEx(node_name.c_str(), flags, node_name.c_str());
	}
}