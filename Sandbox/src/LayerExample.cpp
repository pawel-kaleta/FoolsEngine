#include "LayerExample.h"

LayerExample::LayerExample()
	: Layer("LayerExample"), m_CameraController(1280.0f / 720.0f, true)
{
	//RenderTestSetup(m_Triangle, triangleLayout, triangleVertices,  3 * 3, triangleIndecies,  3);

	m_QuadTextureTint.Texture = fe::TextureLibrary::Get("Default_Texture");
	m_QuadTextureTint.Color = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);
	m_QuadTextureTint.Position = glm::vec2(0.0f, -0.0f);
	m_QuadTextureTint.Size = glm::vec2(0.8f, 0.8f);
	m_QuadTextureTint.Layer = fe::Renderer2D::Layer::L_2;
	m_QuadTextureTint.TextureTilingFactor = 3;
	m_QuadTextureTint.Transparency = false;
	m_QuadTextureTint.Rotation = 20.0f;

	m_QuadColor.Color = glm::vec4(0.9f, 0.2f, 0.9f, 0.8f);
	m_QuadColor.Position = glm::vec2(-0.1f, -0.1f);
	m_QuadColor.Size = glm::vec2(0.4f, 0.4f);
	m_QuadColor.Layer = fe::Renderer2D::Layer::L_1;

	fe::TextureLibrary::Add(fe::Texture2D::Create("assets/textures/Texture_with_Transparency.png"));
	m_QuadTexture.Texture = fe::TextureLibrary::Get("Texture_with_Transparency");
	m_QuadTexture.Position = glm::vec2(0.0f, 0.0f);
	m_QuadTexture.Size = glm::vec2(0.6f, 0.6f);
	m_QuadTexture.Layer = fe::Renderer2D::Layer::L0;
}

void LayerExample::OnUpdate()
{
	FE_PROFILER_FUNC();
	FE_LOG_TRACE("LayerExample::OnUpdate()");
	float dt = fe::Time::DeltaTime();

	if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Right))
	{
		//m_TrianglePosition.x += m_TriangleSpeed * dt;
		m_QuadTexture.Position.x += m_QuadMoveSpeed * dt;
	}
	else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Left))
	{
		//m_TrianglePosition.x -= m_TriangleSpeed * dt;
		m_QuadTexture.Position.x -= m_QuadMoveSpeed * dt;
	}
	if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Up))
	{
		//m_TrianglePosition.y += m_TriangleSpeed * dt;
		m_QuadTexture.Position.y += m_QuadMoveSpeed * dt;
	}
	else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Down))
	{
		//m_TrianglePosition.y -= m_TriangleSpeed * dt;
		m_QuadTexture.Position.y -= m_QuadMoveSpeed * dt;
	}

	//m_Triangle.Transform = glm::translate(glm::mat4(1.0f), m_TrianglePosition);

	m_CameraController.OnUpdate();

	fe::Renderer2D::BeginScene(m_CameraController.GetCamera());
	{
		//fe::Renderer::BeginScene(m_CameraController.GetCamera());
		//fe::Renderer::Draw(m_Triangle.VertexArray, m_Triangle.MaterialInstance, m_Triangle.Transform);
		//fe::Renderer::EndScene();

		fe::Renderer2D::DrawQuad(m_QuadTexture);
		fe::Renderer2D::DrawQuad(m_QuadColor);
		fe::Renderer2D::DrawQuad(m_QuadTextureTint);

	}
	fe::Renderer2D::EndScene();
}

void LayerExample::OnImGuiRender()
{
	ImGui::Begin("Settings");
	//ImGui::ColorEdit4("Triangle color", (float*)m_Triangle.MaterialInstance->GetUniformValuePtr("u_Color"));
	ImGui::ColorEdit4("Quad color", & m_QuadColor.Color.r);

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

void LayerExample::RenderTestSetup()
{
	FE_LOG_DEBUG("RenderTestSetup begin.");

	fe::BufferLayout triangleLayout = {
	{ fe::ShaderData::Type::Float3, "a_Position" }
	};
	float triangleVertices[3 * 3] = {
		-0.5f, -0.5f, 0.5f,
		 0.5f, -0.5f, 0.5f,
		 0.0f,  0.5f, 0.5f
	};
	uint32_t triangleIndecies[3] = { 0, 1, 2 };

	m_Triangle.Transform = glm::mat4(1.0f);

	m_Triangle.VertexArray = fe::VertexArray::Create();
	m_Triangle.VertexArray->Bind();

	m_Triangle.VertexBuffer = fe::VertexBuffer::Create(triangleVertices, sizeof(float) * 3*3);
	m_Triangle.VertexBuffer->SetLayout(triangleLayout);
	m_Triangle.VertexArray->AddVertexBuffer(m_Triangle.VertexBuffer);

	m_Triangle.IndexBuffer = fe::IndexBuffer::Create(triangleIndecies, 3);
	m_Triangle.VertexArray->SetIndexBuffer(m_Triangle.IndexBuffer);


	m_FlatColorMaterial = fe::MaterialLibrary::Get("Flat_Color_Material");
	m_Triangle.MaterialInstance.reset(new fe::MaterialInstance(m_FlatColorMaterial));
	glm::vec4 triangleColor = { 0.8f, 0.1f, 0.1f, 1.0f };
	m_Triangle.MaterialInstance->SetUniformValue(m_FlatColorMaterial->GetUniforms()[0], glm::value_ptr(triangleColor));

	FE_LOG_DEBUG("RenderTestSetup end.");
}

