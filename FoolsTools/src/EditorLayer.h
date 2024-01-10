#pragma once

#include <FoolsEngine.h>
#include "EditorCameraController.h"
#include "Panels\SceneHierarchyPanel.h"
#include "Panels\Inspector.h"

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

		void OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event);

	private:
		Ref<Scene> m_Scene;

		Scope<EditorCameraController>	m_CameraController;
		Scope<Framebuffer>				m_Framebuffer;
		
		glm::vec2	m_ViewportSize = { 0, 0 };
		bool		m_VieportFocus = false;
		bool		m_VieportHover = false;
		
		SceneHierarchyPanel	m_SceneHierarchyPanel;
		Inspector		m_EntityInspector;
		EntityID			m_SelectedEntityID	= NullEntityID;

		void TestSceneSetup();
	};
}