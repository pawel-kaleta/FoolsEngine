#pragma once

#include <FoolsEngine.h>
#include "EditorCameraController.h"
#include "Panels\SceneHierarchyPanel.h"
#include "Panels\ActorInspector.h"
#include "Panels\EntityInspector.h"
#include "Panels\SystemsInspector.h"

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
		
		SceneHierarchyPanel	m_SceneHierarchyPanel;
		ActorInspector		m_ActorInspector;
		EntityInspector		m_EntityInspector;
		SystemsInspector	m_SystemsInspector;
		EntityID			m_SelectedEntityID = NullEntityID;

		void RenderViewport();
		void RenderPanels();
	};
}