#include "LayerExample.h"

LayerExample::LayerExample()
	: Layer("LayerExample"), m_CameraController(1280.0f / 720.0f, true)
{
	fe::BufferLayout triangleLayout = {
		{ fe::ShaderData::Type::Float3, "a_Position" }
	};
	float triangleVertices[3 * 3] = {
		-0.5f, -0.5f, 0.5f,
		 0.5f, -0.5f, 0.5f,
		 0.0f,  0.5f, 0.5f
	};
	uint32_t triangleIndecies[3] = { 0, 1, 2 };
	RenderTestSetup(m_Triangle, triangleLayout, triangleVertices,  3 * 3, triangleIndecies,  3);

	m_FlatColorMaterial = fe::MaterialLibrary::Get("Flat_Color_Material");
	m_Triangle.MaterialInstance.reset(new fe::MaterialInstance(m_FlatColorMaterial));
	glm::vec4 triangleColor = { 0.8f, 0.1f, 0.1f, 1.0f };
	m_Triangle.MaterialInstance->SetUniformValue(m_FlatColorMaterial->GetUniforms()[0], glm::value_ptr(triangleColor));


	fe::BufferLayout rectangleLayout = {
		{ fe::ShaderData::Type::Float3, "a_Position" },
		{ fe::ShaderData::Type::Float2, "a_TexCoord" }
	};
	float rectangleVertices[4 * (3+2)] = {
		-0.6f, -0.6f, 0.0f,  0.0f, 0.0f,
		 0.6f, -0.6f, 0.0f,  1.0f, 0.0f,
		 0.6f,  0.6f, 0.0f,  1.0f, 1.0f,
		-0.6f,  0.6f, 0.0f,  0.0f, 1.0f
	};
	uint32_t rectangleIndecies[3*2] = { 0, 1, 2, 2, 3, 0 };
	RenderTestSetup(m_Rectangle, rectangleLayout, rectangleVertices, 4 * (3+2), rectangleIndecies, 3*2);

	m_TextureMaterial = fe::MaterialLibrary::Get("Basic_Texture_Material");
	m_Rectangle.MaterialInstance.reset(new fe::MaterialInstance(m_TextureMaterial));
	m_Rectangle.MaterialInstance->SetTexture(m_TextureMaterial->GetTextureSlots()[0], fe::TextureLibrary::Get("Default_Texture"));


	float transparentRectangleVertices[4 * (3 + 2)] = {
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
		 0.7f, -0.5f, 0.0f,  1.0f, 0.0f,
		 0.7f,  0.7f, 0.0f,  1.0f, 1.0f,
		-0.5f,  0.7f, 0.0f,  0.0f, 1.0f
	};
	uint32_t transparentRectangleIndecies[3 * 2] = { 0, 1, 2, 2, 3, 0 };
	RenderTestSetup(m_TransparentRectangle, rectangleLayout, transparentRectangleVertices, 4 * (3 + 2), transparentRectangleIndecies, 3 * 2);

	m_TransparentRectangle.MaterialInstance.reset(new fe::MaterialInstance(m_TextureMaterial));
	fe::TextureLibrary::Add(fe::Texture2D::Create("assets/textures/Texture_with_Transparency.png"));
	m_TransparentRectangle.MaterialInstance->SetTexture(m_TextureMaterial->GetTextureSlots()[0], fe::TextureLibrary::Get("Texture_with_Transparency"));
}

void LayerExample::OnUpdate()
{
	FE_PROFILER_FUNC();
	FE_LOG_TRACE("LayerExample::OnUpdate()");

	if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Right))
	{
		m_TrianglePosition.x += m_TriangleSpeed * fe::Time::DeltaTime();
	}
	else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Left))
	{
		m_TrianglePosition.x -= m_TriangleSpeed * fe::Time::DeltaTime();
	}
	if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Up))
	{
		m_TrianglePosition.y += m_TriangleSpeed * fe::Time::DeltaTime();
	}
	else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Down))
	{
		m_TrianglePosition.y -= m_TriangleSpeed * fe::Time::DeltaTime();
	}

	m_Triangle.Transform = glm::translate(glm::mat4(1.0f), m_TrianglePosition);

	m_CameraController.OnUpdate();

	fe::Renderer::BeginScene(m_CameraController.GetCamera());
	{
		FE_LOG_TRACE("RenderTestDraw");

		fe::Renderer::Submit(m_Rectangle.VertexArray, m_Rectangle.MaterialInstance, m_Rectangle.Transform);
		fe::Renderer::Submit(m_TransparentRectangle.VertexArray, m_TransparentRectangle.MaterialInstance, m_TransparentRectangle.Transform);
		fe::Renderer::Submit(m_Triangle.VertexArray, m_Triangle.MaterialInstance, m_Triangle.Transform);
	}
	fe::Renderer::EndScene();

}

void LayerExample::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Triangle color", (float*)m_Triangle.MaterialInstance->GetUniformValuePtr("u_Color"));
	ImGui::End();
}

void LayerExample::OnEvent(fe::Ref<fe::Events::Event> event)
{
	FE_LOG_TRACE("{0}", event);

	m_CameraController.OnEvent(event);

	fe::Events::EventDispacher dispacher(event);
	dispacher.Dispach<fe::Events::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(LayerExample::OnKeyPressedEvent));
}

void LayerExample::OnKeyPressedEvent(fe::Ref<fe::Events::KeyPressedEvent> event)
{
	
}

void LayerExample::RenderTestSetup(
	Sprite& sprite,
	fe::BufferLayout& layout,
	float* vertices, uint32_t verticesNum,
	uint32_t* indecies, uint32_t indeciesNum)
{
	FE_LOG_DEBUG("RenderTestSetup begin.");

	sprite.Transform = glm::mat4(1.0f);

	sprite.VertexArray = fe::VertexArray::Create();
	sprite.VertexArray->Bind();

	sprite.VertexBuffer = fe::VertexBuffer::Create(vertices, sizeof(float) * verticesNum);
	sprite.VertexBuffer->SetLayout(layout);
	sprite.VertexArray->AddVertexBuffer(sprite.VertexBuffer);

	sprite.IndexBuffer = fe::IndexBuffer::Create(indecies, indeciesNum);
	sprite.VertexArray->SetIndexBuffer(sprite.IndexBuffer);

	FE_LOG_DEBUG("RenderTestSetup end.");
}

