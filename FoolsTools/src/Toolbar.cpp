#include "Toolbar.h"

#include "FoolsEngine.h"

namespace fe
{
	Toolbar::Toolbar()
	{
		FE_PROFILER_FUNC();

		Context::Log::Core.Info("Toolbar()"_FE);

		m_IconPlay  = EditorAssetHandle(AssetManager::AssetCreation::EditorAsset<Texture2D>());
		m_IconStop  = EditorAssetHandle(AssetManager::AssetCreation::EditorAsset<Texture2D>());
		m_IconPause = EditorAssetHandle(AssetManager::AssetCreation::EditorAsset<Texture2D>());

		m_IconPlay.Get<Texture2D::Core>().mViewSpecification = {
			.mWrapping = Render::GAPI::Descriptors::Wrapping::Repeat,
			.mFiltering = Render::GAPI::Descriptors::Filtering::Bilinear,
			.mMipmapping = Render::GAPI::Descriptors::Mipmapping::Liniear,
			.mASFiltering = Render::GAPI::Descriptors::ASFiltering::None
		};
		m_IconStop.Get<Texture2D::Core>().mViewSpecification = {
			.mWrapping = Render::GAPI::Descriptors::Wrapping::Repeat,
			.mFiltering = Render::GAPI::Descriptors::Filtering::Bilinear,
			.mMipmapping = Render::GAPI::Descriptors::Mipmapping::Liniear,
			.mASFiltering = Render::GAPI::Descriptors::ASFiltering::None
		};
		m_IconPause.Get<Texture2D::Core>().mViewSpecification = {
			.mWrapping = Render::GAPI::Descriptors::Wrapping::Repeat,
			.mFiltering = Render::GAPI::Descriptors::Filtering::Bilinear,
			.mMipmapping = Render::GAPI::Descriptors::Mipmapping::Liniear,
			.mASFiltering = Render::GAPI::Descriptors::ASFiltering::None
		};

		{
			FE_PROFILER_SCOPE("Loadings");

			TextureLoader::LoadTexture("resources/PlayButton.png", m_IconPlay);
			TextureLoader::LoadTexture("resources/StopButton.png", m_IconStop);
			TextureLoader::LoadTexture("resources/PauseButton.png", m_IconPause);
		}

		auto playGID = m_IconPlay.Get<ACTexture2D_GPU<fe::Render::GAPI::Platform::OpenGL>>().mTextureGID;
		auto stopGID = m_IconStop.Get<ACTexture2D_GPU<fe::Render::GAPI::Platform::OpenGL>>().mTextureGID;
		auto pauseGID = m_IconPause.Get<ACTexture2D_GPU<fe::Render::GAPI::Platform::OpenGL>>().mTextureGID;

		mIconPlayID = Render::GAPI::GetTextureNativeID(playGID);
		mIconStopID = Render::GAPI::GetTextureNativeID(stopGID);
		mIconPauseID = Render::GAPI::GetTextureNativeID(pauseGID);
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


		switch (m_EditorState)
		{
		case EditorState::Edit:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton("m_IconPlay", mIconPlayID, ImVec2(size, size)))
				m_ClickedButton = ToolbarButton::Play;
			break;
		case EditorState::Play:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton("m_IconPause", (ImTextureID)mIconPauseID, ImVec2(size, size)))
				m_ClickedButton = ToolbarButton::Pause;
			ImGui::SameLine();
			if (ImGui::ImageButton("m_IconStop", (ImTextureID)mIconStopID, ImVec2(size, size)))
				m_ClickedButton = ToolbarButton::Stop;
			break;
		case EditorState::Pause:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton("m_IconPlay", (ImTextureID)mIconPlayID, ImVec2(size, size)))
				m_ClickedButton = ToolbarButton::Play;
			ImGui::SameLine();
			if (ImGui::ImageButton("m_IconStop", (ImTextureID)mIconStopID, ImVec2(size, size)))
				m_ClickedButton = ToolbarButton::Stop;
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);

		ImGui::End();
	}
}