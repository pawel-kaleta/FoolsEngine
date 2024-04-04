#include "Toolbar.h"

namespace fe
{
	Toolbar::Toolbar()
	{
		FE_PROFILER_FUNC();

		m_IconPlay  = Ref<Texture>(Texture2D::Create("resources/PlayButton.png" , TextureData::Usage::Map_Albedo));
		m_IconStop  = Ref<Texture>(Texture2D::Create("resources/StopButton.png" , TextureData::Usage::Map_Albedo));
		m_IconPause = Ref<Texture>(Texture2D::Create("resources/PauseButton.png", TextureData::Usage::Map_Albedo));
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

		switch (m_EditorState)
		{
		case EditorState::Edit:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconPlay->GetID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Play;
			break;
		case EditorState::Play:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconPause->GetID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Pause;
			ImGui::SameLine();
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconStop->GetID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Stop;
			break;
		case EditorState::Pause:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconPlay->GetID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Play;
			ImGui::SameLine();
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconStop->GetID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				m_ClickedButton = ToolbarButton::Stop;
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);

		ImGui::End();
	}
}