#include "LayerExample.h"

LayerExample::LayerExample()
	: Layer("LayerExample")
{
	m_Camera = { -1.6f, 1.6f, -0.9f, 0.9f };

	std::string vSourceTriangle = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			uniform vec4 u_Color;

			out vec4 v_Color;

			void main()
			{
				v_Color = u_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";
	std::string fSourceTriangle = R"(
			#version 330 core
			
			layout(location = 0) out vec4 o_color;

			in vec4 v_Color;

			void main()
			{
				o_color = v_Color;
			}
		)";

	m_FlatColorShader.reset(fe::Shader::Create("TestShader", vSourceTriangle, fSourceTriangle));
	m_FlatColorMaterial.reset(new fe::Material(m_FlatColorShader,
	{
		{"u_Color", fe::SDType::Float4}
	}));

	fe::BufferLayout layout = {
		{ fe::SDType::Float3, "a_Position" }
	};
	float triangleVertices[3 * 3] = {
		-0.5f, -0.5f, 0.5f,
		 0.5f, -0.5f, 0.5f,
		 0.0f,  0.5f, 0.5f
	};
	float rectangleVertices[4 * 3] = {
		-0.6f, -0.6f, 0.0f,
		 0.6f, -0.6f, 0.0f,
		 0.6f,  0.6f, 0.0f,
		-0.6f,  0.6f, 0.0f
	};
	uint32_t triangleIndecies[3] = { 0, 1, 2 };
	uint32_t rectangleIndecies[3*2] = { 0, 1, 2, 2, 3, 0 };

	glm::vec4 triangleColor = { 0.8f, 0.1f, 0.1f, 1.0f };
	glm::vec4 rectangleColor = { 0.1f, 0.8f, 0.1f, 1.0f };

	RenderTestSetup(m_Triangle,  layout, triangleColor,  triangleVertices,  3 * 3, triangleIndecies,  3);
	RenderTestSetup(m_Rectangle, layout, rectangleColor, rectangleVertices, 4 * 3, rectangleIndecies, 3*2);
}

void LayerExample::OnUpdate()
{
	FE_PROFILER_FUNC();
	FE_LOG_TRACE("LayerExample::OnUpdate()");
	
	if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Right))
	{
		m_CameraPosition.x += m_CameraSpeed * fe::Time::FrameStep().GetSeconds();
	}
	else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Left))
	{
		m_CameraPosition.x -= m_CameraSpeed * fe::Time::FrameStep().GetSeconds();
	}
	else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Up))
	{
		m_CameraPosition.y += m_CameraSpeed * fe::Time::FrameStep().GetSeconds();
	}
	else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Down))
	{
		m_CameraPosition.y -= m_CameraSpeed * fe::Time::FrameStep().GetSeconds();
	}

	if (fe::InputPolling::IsKeyPressed(fe::InputCodes::D))
	{
		m_TrianglePosition.x += m_TriangleSpeed * fe::Time::FrameStep().GetSeconds();
	}
	else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::A))
	{
		m_TrianglePosition.x -= m_TriangleSpeed * fe::Time::FrameStep().GetSeconds();
	}
	else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::W))
	{
		m_TrianglePosition.y += m_TriangleSpeed * fe::Time::FrameStep().GetSeconds();
	}
	else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::S))
	{
		m_TrianglePosition.y -= m_TriangleSpeed * fe::Time::FrameStep().GetSeconds();
	}

	fe::RenderCommands::Clear();
	fe::RenderCommands::SetClearColor({ 0.1, 0.1, 0.1, 1 });

	m_Camera.SetPosition(m_CameraPosition);
	m_Triangle.Transform = glm::translate(glm::mat4(1.0f), m_TrianglePosition);

	fe::Renderer::BeginScene(m_Camera);
	{
		FE_LOG_TRACE("RenderTestDraw");

		fe::Renderer::Submit(m_Rectangle.VertexArray, m_Rectangle.MaterialInstance, m_Rectangle.Transform);
		fe::Renderer::Submit(m_Triangle.VertexArray, m_Triangle.MaterialInstance, m_Triangle.Transform);
	}
	fe::Renderer::EndScene();
}

void LayerExample::OnEvent(fe::Event& event)
{
	FE_LOG_TRACE("{0}", event);

	fe::EventDispacher dispacher(event);
	dispacher.Dispach<fe::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(LayerExample::OnKeyPressedEvent));
}

bool LayerExample::OnKeyPressedEvent(fe::KeyPressedEvent& event)
{
	return false;
}

void LayerExample::RenderTestSetup(
	Sprite& sprite,
	fe::BufferLayout& layout,
	glm::vec4 color,
	float* vertices, uint32_t verticesNum,
	uint32_t* indecies, uint32_t indeciesNum)
{
	FE_LOG_DEBUG("RenderTestSetup begin.");

	sprite.Transform = glm::mat4(1.0f);

	sprite.VertexArray.reset(fe::VertexArray::Create());
	sprite.VertexArray->Bind();

	sprite.VertexBuffer.reset(fe::VertexBuffer::Create(vertices, sizeof(float) * verticesNum));
	sprite.VertexBuffer->SetLayout(layout);
	sprite.VertexArray->AddVertexBuffer(sprite.VertexBuffer);

	sprite.IndexBuffer.reset(fe::IndexBuffer::Create(indecies, indeciesNum));
	sprite.VertexArray->SetIndexBuffer(sprite.IndexBuffer);

	sprite.MaterialInstance.reset(new fe::MaterialInstance(m_FlatColorMaterial));
	sprite.MaterialInstance->SetUniformValue(m_FlatColorMaterial->GetUniforms()[0].GetName(), glm::value_ptr(color));

	FE_LOG_DEBUG("RenderTestSetup end.");
}
