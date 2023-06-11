#pragma once

#include <FoolsEngine.h>

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
		Ref<Framebuffer> m_Framebuffer;
		OrthographicCameraController m_CameraController;

		bool m_VieportFocus = false;
		bool m_VieportHover = false;

		Scope<Scene> m_Scene;

		Set m_Target;
		float m_TargetMoveSpeed = 1.1f;
		Set m_ColorQuad;
	};
}