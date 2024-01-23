#pragma once

#include <FoolsEngine.h>
#include "EditorCameraController.h"
#include "Panels\WorldHierarchyPanel.h"
#include "Panels\ActorInspector.h"
#include "Panels\EntityInspector.h"
#include "Panels\SystemsInspector.h"
#include <filesystem>

namespace fe
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		virtual void OnAttach()							override;
		virtual void OnUpdate()							override;
		virtual void OnImGuiRender()					override;
		virtual void OnEvent(Ref<Events::Event> event)	override;

	private:
		Ref<Scene> m_Scene;

		Scope<EditorCameraController>	m_CameraController;
		Scope<Framebuffer>				m_Framebuffer;
		
		glm::vec2	m_ViewportSize = { 0, 0 };
		bool		m_VieportFocus = false;
		bool		m_VieportHover = false;

		enum class SceneState
		{
			Edit,
			Play,
			Pause
		} m_SceneState = SceneState::Edit;
		
		struct Panels
		{
			WorldHierarchyPanel	WorldHierarchyPanel;
			ActorInspector		ActorInspector;
			EntityInspector		EntityInspector;
			SystemsInspector	SystemsInspector;
		} m_Panels;

		EntityID m_SelectedEntityID = NullEntityID;

		Ref<Texture> m_IconPlay;
		Ref<Texture> m_IconPause;
		Ref<Texture> m_IconStop;

		void RenderViewport();
		void RenderPanels();
		void RenderMainMenu();
		void RenderToolbar();

		void NewScene();
		void OpenScene();
		void SaveScene(const Ref<Scene>& scene);
		void SaveSceneAs(const Ref<Scene>& scene);

		void SetSceneContext(const Ref<Scene>& scene);
		void SetSelectionContext(EntityID entityID);
		void GetSelection();

		void OnScenePlay();
		void OnSceneStop();
		void OnScenePause();

		void OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event);
	};
}