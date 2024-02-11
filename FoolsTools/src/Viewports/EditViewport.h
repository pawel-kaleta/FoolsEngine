#pragma once

#include <FoolsEngine.h>
#include "EditorCameraController.h"

#include <ImGuizmo.h>

namespace fe
{
	class EditViewport
	{
	public:
		EditViewport();

		void OnUpdate();
		void RenderScene();
		void OnEvent(Ref<Events::Event> event);
		void OnImGuiRender();

		void SetScene(const Ref<Scene>& scene) { m_Scene = scene; }
		EditorCameraController& GetCameraController() { return *m_CameraController.get(); }

		void     SetSelection(EntityID entityID) { m_SelectedEntityID = entityID; }
		EntityID GetSelectionRequest() { return m_EntityIDSelectionRequest; }
	private:
		void OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event);
		void OnMouseButtonPressedEvent(Ref<Events::MouseButtonPressedEvent> event);
		void RenderGuizmos();
		EntityID ReadEntityIDfromBuffer();

		Ref<Scene> m_Scene;
		bool m_IsVisible = false;
		bool m_EntityClicked = false;

		EntityID m_SelectedEntityID = NullEntityID;
		EntityID m_EntityIDSelectionRequest = NullEntityID;

		glm::vec2 m_ViewportSize = { 0, 0 };
		glm::vec2 m_ViewportBounds[2] = { { 0, 0 }, { 0, 0 } };
		bool      m_VieportFocus = false;
		bool      m_VieportHover = false;

		ImGuizmo::OPERATION m_GuizmoType = ImGuizmo::OPERATION::UNIVERSAL;
		Scope<EditorCameraController> m_CameraController;

		Scope<Framebuffer> m_Framebuffer;
	};
}