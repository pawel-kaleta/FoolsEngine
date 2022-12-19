#include "LayerExample.h"

LayerExample::LayerExample()
	: Layer("LayerExample")
{
	m_Camera = { -1.6f, 1.6f, -0.9f, 0.9f };


	fe::BufferLayout triangle_Layout = {
		{ fe::SDType::Float3, "a_Position" },
		{ "a_Color", fe::SDPrimitive::Float, 4 }
	};
	fe::BufferLayout rectangle_Layout = {
		{ fe::SDType::Float3, "a_Position" },
		{ fe::SDType::Float4, "a_Color" }
	};
	std::string vSourceTriangle = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";
	std::string fSourceTriangle = R"(
			#version 330 core
			
			layout(location = 0) out vec4 o_color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				o_color = vec4(v_Position + 0.5, 1.0);
				o_color = v_Color;
			}
		)";
	std::string vSourceRectangle = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";
	std::string fSourceRectangle = R"(
			#version 330 core
			
			layout(location = 0) out vec4 o_color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				o_color = vec4(v_Position + 0.5, 1.0);
				o_color = v_Color;
			}
		)";
	float triangleVertices[3 * (3 + 4)] = {
		-0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 1.0f
	};
	float rectangleVertices[4 * (3 + 4)] = {
		-0.6f, -0.6f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
		 0.6f, -0.6f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f,
		 0.6f,  0.6f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-0.6f,  0.6f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f
	};
	uint32_t triangleIndecies[3] = { 0, 1, 2 };
	uint32_t rectangleIndecies[6] = { 0, 1, 2, 2, 3, 0 };

	RenderTestSetup(m_Triangle, triangle_Layout, vSourceTriangle, fSourceTriangle, triangleVertices, 3 * (3 + 4), triangleIndecies, 3);
	RenderTestSetup(m_Rectangle, rectangle_Layout, vSourceRectangle, fSourceRectangle, rectangleVertices, 4 * (3 + 4), rectangleIndecies, 6);
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

		fe::Renderer::Submit(m_Rectangle.VertexArray, m_Rectangle.Shader, m_Rectangle.Transform);
		fe::Renderer::Submit(m_Triangle.VertexArray, m_Triangle.Shader, m_Triangle.Transform);
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
	std::string& vertexSource, std::string& fragmentSource,
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

	sprite.Shader.reset(fe::Shader::Create("TestShader", vertexSource, fragmentSource));

	FE_LOG_DEBUG("RenderTestSetup end.");
}
