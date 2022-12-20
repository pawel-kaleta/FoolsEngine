#pragma once

#include <FoolsEngine.h>

class LayerExample : public fe::Layer
{
public:
	LayerExample();

	struct Sprite
	{
		std::shared_ptr<fe::MaterialInstance> MaterialInstance;
		std::shared_ptr<fe::VertexArray> VertexArray;
		std::shared_ptr<fe::VertexBuffer> VertexBuffer;
		std::shared_ptr<fe::IndexBuffer> IndexBuffer;
		glm::mat4 Transform = glm::mat4(1.0f);
	};

	void OnUpdate() override;
	void OnEvent(fe::Event& event) override;
	bool OnKeyPressedEvent(fe::KeyPressedEvent& event);

private:
	fe::OrtographicCamera m_Camera;
	glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
	float m_CameraSpeed = 1.0f;
	glm::vec3 m_TrianglePosition = { 0.0f, 0.0f, 0.0f };
	float m_TriangleSpeed = 1.2f;
	std::shared_ptr<fe::Shader> m_FlatColorShader;
	std::shared_ptr<fe::Material> m_FlatColorMaterial;
	

	
	void RenderTestSetup(
		Sprite& sprite,
		fe::BufferLayout& layout,
		glm::vec4 color,
		float* vertices, uint32_t verticesNum,
		uint32_t* indecies, uint32_t indeciesNum);

	Sprite m_Triangle;
	Sprite m_Rectangle;
};