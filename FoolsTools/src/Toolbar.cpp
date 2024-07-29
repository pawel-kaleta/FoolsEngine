#include "Toolbar.h"

#include "FoolsEngine.h"

namespace fe
{
	Toolbar::Toolbar()
	{
		FE_PROFILER_FUNC();

		m_IconPlay  = EditorAssetHandle<Texture2D>(EditorAssetManager::NewAsset<Texture2D>());
		m_IconStop  = EditorAssetHandle<Texture2D>(EditorAssetManager::NewAsset<Texture2D>());
		m_IconPause = EditorAssetHandle<Texture2D>(EditorAssetManager::NewAsset<Texture2D>());

		auto GDI = Renderer::GetActiveGDItype();

		auto playUser = m_IconPlay.Use();
		auto stopUser = m_IconStop.Use();
		auto pauseUser = m_IconPause.Use();

		TextureLoader::LoadTexture("resources/PlayButton.png" , playUser);
		TextureLoader::LoadTexture("resources/StopButton.png" , stopUser);
		TextureLoader::LoadTexture("resources/PauseButton.png", pauseUser);

		playUser.CreateGDITexture2D<OpenGLTexture2D>();
		stopUser.CreateGDITexture2D<OpenGLTexture2D>();
		pauseUser.CreateGDITexture2D<OpenGLTexture2D>();
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
			if (ImGui::ImageButton((ImTextureID)(uint64_t)(m_IconPlay.Observe().GetRendererID(GDI)), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Play;
			break;
		case EditorState::Play:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(uint64_t)(m_IconPause.Observe().GetRendererID(GDI)), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Pause;
			ImGui::SameLine();
			if (ImGui::ImageButton((ImTextureID)(uint64_t)(m_IconStop.Observe().GetRendererID(GDI)), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Stop;
			break;
		case EditorState::Pause:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(uint64_t)(m_IconPlay.Observe().GetRendererID(GDI)), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Play;
			ImGui::SameLine();
			if (ImGui::ImageButton((ImTextureID)(uint64_t)(m_IconStop.Observe().GetRendererID(GDI)), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Stop;
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);

		ImGui::End();
	}
}