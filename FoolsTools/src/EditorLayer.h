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
		OrthographicCameraController m_CameraController;

		glm::vec3 m_TrianglePosition = { 0.0f, 0.0f, 0.0f };
		float m_TriangleSpeed = 1.2f;
		Ref<Material> m_FlatColorMaterial;

		Renderer2D::Quad m_QuadColor;
		Renderer2D::Quad m_QuadTexture;
		Renderer2D::Quad m_QuadTextureTint;
		float m_QuadMoveSpeed = 1.1f;

		Ref<Framebuffer> m_Framebuffer;
	};

}