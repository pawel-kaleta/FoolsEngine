#include "Toolbar.h"

#include "FoolsEngine.h"

namespace fe
{
	Toolbar::Toolbar()
	{
		FE_PROFILER_FUNC();

		m_IconPlay  = EditorAssetHandle<Texture2D>(AssetManager::CreateInternalAsset<Texture2D>());
		m_IconStop  = EditorAssetHandle<Texture2D>(AssetManager::CreateInternalAsset<Texture2D>());
		m_IconPause = EditorAssetHandle<Texture2D>(AssetManager::CreateInternalAsset<Texture2D>());

		TextureLoader::LoadTexture("resources/PlayButton.png" , m_IconPlay);
		TextureLoader::LoadTexture("resources/StopButton.png" , m_IconStop);
		TextureLoader::LoadTexture("resources/PauseButton.png", m_IconPause);

		auto GDI = Renderer::GetActiveGDItype();
		m_IconPlay.CreateGDITexture2D(GDI);
		m_IconStop.CreateGDITexture2D(GDI);
		m_IconPause.CreateGDITexture2D(GDI);
	}

	void Toolbar::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

		m_ClickedButton = ToolbarButton::None;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f; //headbar

		auto GDI = Renderer::GetActiveGDItype();

		switch (m_EditorState)
		{
		case EditorState::Edit:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(uint64_t)(m_IconPlay.GetRendererID(GDI)), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Play;
			break;
		case EditorState::Play:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(uint64_t)(m_IconPause.GetRendererID(GDI)), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Pause;
			ImGui::SameLine();
			if (ImGui::ImageButton((ImTextureID)(uint64_t)(m_IconStop.GetRendererID(GDI)), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Stop;
			break;
		case EditorState::Pause:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(uint64_t)(m_IconPlay.GetRendererID(GDI)), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Play;
			ImGui::SameLine();
			if (ImGui::ImageButton((ImTextureID)(uint64_t)(m_IconStop.GetRendererID(GDI)), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Stop;
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);

		ImGui::End();
	}
}