#include "ContentBrowser.h"

#include <FoolsEngine.h>

#include "AssetImport\FileHandler.h"

namespace fe
{
	glm::vec4 AssetTypeColors[AssetType::Count];
	glm::vec4 LoaderColors[LoaderType::Count];

	void InitColors()
	{
		//https://flatuicolors.com/palette/us

		for (auto& color : AssetTypeColors) color = { 1, 1, 1, 1 };
		AssetTypeColors[AssetType::Texture2D	] = FE_RGBA(  0, 184, 148, 1.0);
		AssetTypeColors[AssetType::Shader		] = FE_RGBA(  0, 206, 201, 1.0);
		AssetTypeColors[AssetType::ShadingModel	] = FE_RGBA(  0, 206, 201, 1.0);
		AssetTypeColors[AssetType::Mesh			] = FE_RGBA(  9, 132, 227, 1.0);
		AssetTypeColors[AssetType::RenderMesh	] = FE_RGBA(  9, 132, 227, 1.0);
		AssetTypeColors[AssetType::Model		] = FE_RGBA(  9, 132, 227, 1.0);
		AssetTypeColors[AssetType::Scene		] = FE_RGBA(108,  92, 231, 1.0);

		for (auto& color : LoaderColors) color = { 1, 1, 1, 1 };
		LoaderColors[LoaderType::Texture	] = FE_RGBA( 85, 239, 196, 1.0);
		LoaderColors[LoaderType::Shader		] = FE_RGBA(129, 236, 236, 1.0);
		LoaderColors[LoaderType::Geometry	] = FE_RGBA(116, 185, 255, 1.0);
	}
	
	ContentBrowser::ContentBrowser()
	{
		FE_PROFILER_FUNC();

		m_Icons.File = EditorAssetHandle<Texture2D>(AssetManager::AssetCreation::EditorAsset<Texture2D>());
		m_Icons.Folder = EditorAssetHandle<Texture2D>(AssetManager::AssetCreation::EditorAsset<Texture2D>());

		TextureLoader::LoadTexture("resources/File.png", m_Icons.File);
		TextureLoader::LoadTexture("resources/Folder.png", m_Icons.Folder);

		auto GDI = Renderer::GetActiveGDItype();
		m_Icons.File.CreateGDITexture2D(GDI);
		m_Icons.Folder.CreateGDITexture2D(GDI);

		m_Icons.FileID   = (void*)m_Icons.File.GetRendererID(GDI);
		m_Icons.FolderID = (void*)m_Icons.Folder.GetRendererID(GDI);

		InitColors();
	}

	void ContentBrowser::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Content Browser");
		ImGui::PopStyleVar();

		// Render Folder Hierarchy
		{
			FE_PROFILER_SCOPE("Render Folder Hierarchy");

			if (ImGui::BeginChild("Folders Hierarchy", ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border))
			{
				ImGui::Checkbox("Display Files", &(m_Settings.DisplayFiles));
				
				RenderFolderNode(std::filesystem::directory_entry(m_AssetsPath));

				ImGui::EndChild();
			}
		}

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, { 0,0 });
		ImGui::SameLine();
		ImGui::PopStyleVar();

		if (ImGui::BeginChild("Folder's Content", { 0,0 }, ImGuiChildFlags_Border))
		{
			FE_PROFILER_SCOPE("Render Folder Content");

			ImGui::Checkbox("Display Directories", &(m_Settings.DisplayDirectories));
			ImGui::SameLine();
			ImGui::SetNextItemWidth(272);
			ImGui::SliderInt("Thumbnail Size", (int*)&(m_Settings.ThumbnailSize), 16, 256);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.29f, 0.48f, 0.3f));

			if (m_Settings.DisplayDirectories)
			{
				if (m_CurrentPath.compare(m_AssetsPath)) // skip if most outer folder
					RenderUPFolder();

				Scratchpad sp;
				auto file_names = sp.NewObject<std::pmr::vector<std::pmr::string>>();

				// RenderFolders iterates over directory rendering folders and creating list of filenames
				// RenderFiles iterates over filenames in a list
				// this avoids iterating directory twice - expensive (OSI)

				RenderFolders(&sp, file_names);

				for (const auto& file : *file_names)
				{
					Scratchpad sp2;

					const auto dot_pos = file.rfind(".");
					const std::pmr::string stem(file.c_str(), dot_pos, &sp2);
					const std::pmr::string extension(file.c_str() + dot_pos, file.length() - dot_pos, &sp2);

					RenderFile(stem, extension);
				}
			}
			else
			{
				Scratchpad sp;

				for (auto& dir_entry : std::filesystem::directory_iterator(m_CurrentPath))
				{
					if (dir_entry.is_directory())
						continue;

					const auto stem = dir_entry.path().stem().string<PMR_STRING_TEMPLATE_PARAMS>(&sp);
					const auto extension = dir_entry.path().extension().string<PMR_STRING_TEMPLATE_PARAMS>(&sp);

					RenderFile(stem, extension);
				}
			}

			ImGui::PopStyleColor(2);

			ImGui::EndChild();
		}

		ImGui::End();
	}

	void ContentBrowser::RenderFile(const std::pmr::string& stem, const std::pmr::string& extension)
	{
		FE_PROFILER_FUNC();

		Scratchpad sp;
		auto& tnSize = m_Settings.ThumbnailSize;
		ImVec2 thumbnailSizeIm((float)tnSize, (float)tnSize);
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		ImGuiStyle& style = ImGui::GetStyle();

		auto loader_registry_item_ptr = LoadersRegistry::GetItem(extension);
		auto asset_types_registry_item_ptr = AssetTypesRegistry::GetItem(extension);

		const std::pmr::string file_name(stem + extension, &sp);
		const std::filesystem::path file_path = m_CurrentPath / file_name;

		if (loader_registry_item_ptr || asset_types_registry_item_ptr)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_CellPadding, { 0.f,0.f });
			ImGui::BeginTable(stem.c_str(), 1, ImGuiTableFlags_BordersOuter, { (float)(tnSize + 9), 0 });
			ImGui::PopStyleVar();
			ImGui::TableNextColumn();

			// thumbnail in the future

			ImGui::ImageButton(file_name.c_str(), m_Icons.FileID, thumbnailSizeIm, { 0,1 }, { 1,0 });

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				// Set payload to carry the asset filepath of our item (could be anything)
				static std::filesystem::path payload;
				payload = file_path;
				ImGui::SetDragDropPayload("AssetPath", &payload, sizeof(payload));

				// Display preview (could be anything, e.g. when dragging an image we could decide to display
				// the filename and a small preview of the image, etc.)
				ImGui::ImageButton(file_name.c_str(), m_Icons.FileID, { 32,32 }, { 0,1 }, { 1,0 });
				ImGui::Text(stem.c_str());

				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemClicked(0) && ImGui::IsMouseDoubleClicked(0) && loader_registry_item_ptr)
			{
				FileHandler::OpenFile(file_path, loader_registry_item_ptr);
			}

			ImGui::BeginTable(file_name.c_str(), 1, ImGuiTableFlags_PadOuterX);
			ImGui::TableNextRow(ImGuiTableRowFlags_None, 25.f);
			ImGui::TableNextColumn();
			ImGui::TextWrapped(stem.c_str());
			ImGui::TableNextColumn();

			std::pmr::string alias(&sp);

			if (loader_registry_item_ptr)
			{
				alias = loader_registry_item_ptr->SourceExtensionAlias;
				ImGui::PushStyleColor(ImGuiCol_Text, *(ImVec4*)&LoaderColors[loader_registry_item_ptr->Type.ToInt()]);
				ImGui::TextWrapped(alias.c_str());
				ImGui::PopStyleColor();
			}
			else if (asset_types_registry_item_ptr)
			{
				alias = asset_types_registry_item_ptr->TypeConstCharPtr;
				ImGui::PushStyleColor(ImGuiCol_Text, *(ImVec4*)&AssetTypeColors[asset_types_registry_item_ptr->Type.ToInt()]);
				ImGui::TextWrapped(alias.c_str());
				ImGui::PopStyleColor();
			}
			else
			{
				FE_CORE_ASSERT(false, "How did we got here?");

				auto error_color = FE_RGBA(214, 48, 49, 1.0);
				ImGui::PushStyleColor(ImGuiCol_Text, *(ImVec4*)&error_color);
				ImGui::TextWrapped("Error");
				ImGui::PopStyleColor();
			}

			ImGui::EndTable();

			ImGui::EndTable();
		}
		else
		{
			ImGui::BeginTable(file_name.c_str(), 1, 0, { (float)(tnSize + 9), 0 });
			ImGui::TableNextColumn();
			ImGui::ImageButton(file_name.c_str(), m_Icons.FileID, thumbnailSizeIm, { 0,1 }, { 1,0 });
			ImGui::TableNextColumn();
			ImGui::TextWrapped(file_name.c_str());
			ImGui::EndTable();
		}

		float last_button_x2 = ImGui::GetItemRectMax().x;
		float next_button_x2 = last_button_x2 + style.ItemSpacing.x + tnSize + 9; // Expected position if next button was on same line
		if (next_button_x2 < window_visible_x2)
			ImGui::SameLine();
	}

	void ContentBrowser::RenderUPFolder()
	{
		FE_PROFILER_FUNC();

		auto& tnSize = m_Settings.ThumbnailSize;
		ImVec2 thumbnailSizeIm((float)tnSize, (float)tnSize);
		ImGuiStyle& style = ImGui::GetStyle();
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_CellPadding, { 0.f,0.f });
		ImGui::BeginTable("UP", 1, 0, { (float)(tnSize + 9), 0 });
		ImGui::TableNextColumn();
		ImGui::ImageButton("UP2", m_Icons.FolderID, thumbnailSizeIm, { 0,1 }, { 1,0 });
		ImGui::PopStyleVar();

		if (ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)
			&&
			ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			m_CurrentPath = m_CurrentPath.parent_path();
		}

		ImGui::TableNextColumn();
		ImGui::Text("..");
		ImGui::EndTable();

		float last_button_x2 = ImGui::GetItemRectMax().x;
		float next_button_x2 = last_button_x2 + style.ItemSpacing.x + tnSize + 9; // Expected position if next element was on same line
		if (next_button_x2 < window_visible_x2)
			ImGui::SameLine();
	}

	void ContentBrowser::RenderFolders(Scratchpad* sp, std::pmr::vector<std::pmr::string>* file_names)
	{
		FE_PROFILER_FUNC();

		auto& tnSize = m_Settings.ThumbnailSize;
		ImVec2 thumbnailSizeIm((float)tnSize, (float)tnSize);
		ImGuiStyle& style = ImGui::GetStyle();
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

		for (auto& dir_entry : std::filesystem::directory_iterator(m_CurrentPath))
		{
			FE_PROFILER_SCOPE("Dir Entry");

			auto entry_name = dir_entry.path().filename().string<PMR_STRING_TEMPLATE_PARAMS>(sp);

			if (dir_entry.is_directory())
			{
				ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_CellPadding, { 0.f,0.f });
				ImGui::BeginTable(entry_name.c_str(), 1, 0, { (float)(tnSize + 9), 0 });
				ImGui::TableNextColumn();
				ImGui::ImageButton(entry_name.c_str(), m_Icons.FolderID, thumbnailSizeIm, { 0,1 }, { 1,0 });
				ImGui::PopStyleVar();

				if (ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)
					&&
					ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
				{
					m_CurrentPath /= entry_name;
				}

				ImGui::TableNextColumn();
				ImGui::Text(entry_name.c_str());
				ImGui::EndTable();

				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + tnSize + 9; // Expected position if next button was on same line
				if (next_button_x2 < window_visible_x2)
					ImGui::SameLine();
			}
			else
			{
				file_names->push_back(std::move(entry_name));
			}
		}
	}

	void ContentBrowser::RenderFolderNode(const std::filesystem::directory_entry& dir)
	{
		FE_PROFILER_FUNC();

		if (dir.path().compare(m_CurrentPath) != 0 && m_Settings.DisplayDirectories)
			ImGui::SetNextItemOpen(false);
		if (dir.path().lexically_relative(m_CurrentPath).compare("..") == 0) // this is expensive
			ImGui::SetNextItemOpen(true);
		if (dir.path().compare(m_CurrentPath) == 0)
			ImGui::SetNextItemOpen(true);
		if (dir.path().compare(m_AssetsPath) == 0)
			ImGui::SetNextItemOpen(true);


		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		bool selected = (m_CurrentPath == dir);
		flags |= selected ? ImGuiTreeNodeFlags_Selected : 0;

		Scratchpad sp;
		auto node_name = dir.path().filename().string<PMR_STRING_TEMPLATE_PARAMS>(&sp);
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

				// Render File Node
				{
					FE_PROFILER_SCOPE("Render File Node");

					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
					flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;// | ImGuiTreeNodeFlags_Bullet;

					Scratchpad sp2;
					auto node_name = p.path().filename().string<PMR_STRING_TEMPLATE_PARAMS>(&sp2);
					ImGui::TreeNodeEx(node_name.c_str(), flags, node_name.c_str());
				}
			}
		}

		ImGui::TreePop();
	}
}