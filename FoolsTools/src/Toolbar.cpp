#include "Toolbar.h"

#include "FoolsEngine.h"

namespace fe
{
	Toolbar::Toolbar()
	{
		FE_PROFILER_FUNC();

		m_IconPlay  = EditorAssetHandle<Texture2D>(AssetManager::AssetCreation::EditorAsset<Texture2D>());
		m_IconStop  = EditorAssetHandle<Texture2D>(AssetManager::AssetCreation::EditorAsset<Texture2D>());
		m_IconPause = EditorAssetHandle<Texture2D>(AssetManager::AssetCreation::EditorAsset<Texture2D>());

		TextureLoader::LoadTexture("resources/PlayButton.png" , m_IconPlay);
		TextureLoader::LoadTexture("resources/StopButton.png" , m_IconStop);
		TextureLoader::LoadTexture("resources/PauseButton.png", m_IconPause);

		auto GAPI = Renderer::GetActiveGAPIType();
		m_IconPlay.SendDataToGPU(GAPI);
		m_IconStop.SendDataToGPU(GAPI);
		m_IconPause.SendDataToGPU(GAPI);
	}

	void Toolbar::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

		m_ClickedButton = ToolbarButton::None;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& button_hovered_color = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(button_hovered_color.x, button_hovered_color.y, button_hovered_color.z, 0.5f));
		const auto& button_active_color = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(button_active_color.x, button_active_color.y, button_active_color.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetContentRegionAvail().y - 6.0f; // padding bottom and top

		auto GAPI = Renderer::GetActiveGAPIType();

		switch (m_EditorState)
		{
		case EditorState::Edit:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton("m_IconPlay", (ImTextureID)m_IconPlay.GetGAPIResourceID(GAPI), ImVec2(size, size)))
				m_ClickedButton = ToolbarButton::Play;
			break;
		case EditorState::Play:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton("m_IconPause", (ImTextureID)m_IconPause.GetGAPIResourceID(GAPI), ImVec2(size, size)))
				m_ClickedButton = ToolbarButton::Pause;
			ImGui::SameLine();
			if (ImGui::ImageButton("m_IconStop", (ImTextureID)m_IconStop.GetGAPIResourceID(GAPI), ImVec2(size, size)))
				m_ClickedButton = ToolbarButton::Stop;
			break;
		case EditorState::Pause:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton("m_IconPlay", (ImTextureID)m_IconPlay.GetGAPIResourceID(GAPI), ImVec2(size, size)))
				m_ClickedButton = ToolbarButton::Play;
			ImGui::SameLine();
			if (ImGui::ImageButton("m_IconStop", (ImTextureID)m_IconStop.GetGAPIResourceID(GAPI), ImVec2(size, size)))
				m_ClickedButton = ToolbarButton::Stop;
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);

		ImGui::End();
	}
}