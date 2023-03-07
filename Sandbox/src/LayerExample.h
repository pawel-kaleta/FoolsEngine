#pragma once

#include <FoolsEngine.h>

class LayerExample : public fe::Layer
{
public:
	LayerExample();

	struct Sprite
	{
		fe::Ref<fe::MaterialInstance> MaterialInstance;
		fe::Ref<fe::VertexArray> VertexArray;
		fe::Ref<fe::VertexBuffer> VertexBuffer;
		fe::Ref<fe::IndexBuffer> IndexBuffer;
		glm::mat4 Transform = glm::mat4(1.0f);
	};

	void OnUpdate() override;
	void OnEvent(fe::Ref<fe::Events::Event> event) override;
	void OnKeyPressedEvent(fe::Ref<fe::Events::KeyPressedEvent> event);
	void OnImGuiRender() override;

private:
	fe::OrthographicCameraController m_CameraController;

	glm::vec3 m_TrianglePosition = { 0.0f, 0.0f, 0.0f };
	float m_TriangleSpeed = 1.2f;
	fe::Ref<fe::Material> m_FlatColorMaterial;

	void RenderTestSetup();

	Sprite m_Triangle;

	fe::Renderer2D::Quad m_QuadColor;
	fe::Renderer2D::Quad m_QuadTexture;
	fe::Renderer2D::Quad m_QuadTextureTint;
	float m_QuadMoveSpeed = 1.1f;
};