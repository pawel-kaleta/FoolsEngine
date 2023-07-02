#pragma once

#include <FoolsEngine.h>
#include "EditorCameraController.h"
#include "Panels\SceneHierarchyPanel.h"

namespace fe
{

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		virtual void OnAttach() override;

		void OnUpdate() override;
		void OnEvent(Ref<Events::Event> event) override;
		void OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event);
		void OnImGuiRender() override;

	private:
		glm::vec2 m_ViewportSize = { 0, 0 };
		Scope<Framebuffer> m_Framebuffer;
		EditorCameraController m_CameraController;
		SceneHierarchyPanel m_SceneHierarchyPanel;

		bool m_VieportFocus = false;
		bool m_VieportHover = false;

		Ref<Scene> m_Scene;

		Set m_ColorQuad;
	};
}