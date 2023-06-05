#pragma once

#include <FoolsEngine.h>

class LayerExample : public fe::Layer
{
public:
	LayerExample();

	virtual void OnAttach() override;

	void OnUpdate() override;
	void OnEvent(fe::Ref<fe::Events::Event> event) override;
	void OnKeyPressedEvent(fe::Ref<fe::Events::KeyPressedEvent> event);
	void OnImGuiRender() override;

private:
	fe::OrthographicCameraController m_CameraController;

	glm::vec3 m_TrianglePosition = { 0.0f, 0.0f, 0.0f };
	float m_TriangleSpeed = 1.2f;
	fe::Ref<fe::Material> m_FlatColorMaterial;

	fe::Renderer2D::Quad m_QuadColor;
	fe::Renderer2D::Quad m_QuadTexture;
	fe::Renderer2D::Quad m_QuadTextureTint;
	float m_QuadMoveSpeed = 1.1f;
};