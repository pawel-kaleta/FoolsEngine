#pragma once

#include <FoolsEngine.h>
#include "EditorCameraController.h"
#include "Panels\SceneHierarchyPanel.h"
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
		std::filesystem::path m_SceneFilepath;

		Scope<EditorCameraController>	m_CameraController;
		Scope<Framebuffer>				m_Framebuffer;
		
		glm::vec2	m_ViewportSize = { 0, 0 };
		bool		m_VieportFocus = false;
		bool		m_VieportHover = false;
		
		SceneHierarchyPanel	m_SceneHierarchyPanel;
		ActorInspector		m_ActorInspector;
		EntityInspector		m_EntityInspector;
		SystemsInspector	m_SystemsInspector;
		EntityID			m_SelectedEntityID = NullEntityID;

		void RenderViewport();
		void RenderPanels();
		void RenderMainMenu();

		void NewScene();
		void OpenScene();
		void SaveScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SetSceneContext(const Ref<Scene>& scene);

		void OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event);
	};
}