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
	fe::OrtographicCamera m_Camera;
	glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
	float m_CameraSpeed = 1.0f;
	glm::vec3 m_TrianglePosition = { 0.0f, 0.0f, 0.0f };
	float m_TriangleSpeed = 1.2f;
	fe::Ref<fe::Material> m_FlatColorMaterial;
	fe::Ref<fe::Material> m_TextureMaterial;

	void RenderTestSetup(
		Sprite& sprite,
		fe::BufferLayout& layout,
		float* vertices, uint32_t verticesNum,
		uint32_t* indecies, uint32_t indeciesNum);

	Sprite m_Triangle;
	Sprite m_Rectangle;
	Sprite m_TransparentRectangle;
};