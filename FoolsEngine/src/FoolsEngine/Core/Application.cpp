#include "FE_pch.h"

#include "FoolsEngine\Core\Application.h"
#include "FoolsEngine\Platform\Win10\Win10Window.h"
#include "FoolsEngine\Core\InputPolling.h"

#include <glad\glad.h>
#include "FoolsEngine\Platform\OpenGL\OpenGLBuffers.h"

namespace fe {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(!s_Instance, "Application already exists!")
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(std::bind(&MainEventDispacher::ReceiveEvent, & m_MainEventDispacher, std::placeholders::_1));

		m_AppLayer = std::make_shared<ApplicationLayer>(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		m_LayerStack.PushOuterLayer(m_AppLayer);

		m_ImGuiLayer = std::make_shared<ImGuiLayer>();
		m_LayerStack.PushOuterLayer(m_ImGuiLayer);


		TriangleTestSetup();
	}

	Application::~Application()
	{
		FE_PROFILER_FUNC();
	}

	void Application::OnEvent(std::shared_ptr<Event> event)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_TRACE("Application::OnEvent");
		FE_LOG_CORE_TRACE(event->GetEventType());
		FE_LOG_CORE_TRACE(event->GetName());
		FE_LOG_CORE_TRACE(event->GetCategoryFlags());

		if (event->GetEventType() == EventType::WindowClose)
		{
			OnWindowCloseEvent(event);
		}
	}

	void Application::OnWindowCloseEvent(std::shared_ptr<Event> event)
	{
		FE_PROFILER_FUNC();

		m_Running = false;
		event->Handled = true;
		FE_LOG_CORE_INFO("Window Close Event");
	}


	void Application::Run()
	{
		FE_PROFILER_FUNC();

		while (m_Running)
		{
			m_Window->OnUpdate();

			TriangleTestDraw();

			UpdateLayers();
			UpdateImGui();

			auto [x, y] = InputPolling::GetMousePosition();
			FE_LOG_CORE_TRACE("Mouse position: {0}, {1}", x, y);

			m_MainEventDispacher.DispachEvents(m_LayerStack);
		}
		
		Log::SetClientLoggingLevel(spdlog::level::trace);
		Log::SetCoreLoggingLevel(spdlog::level::trace);
	}

	void Application::UpdateLayers()
	{
		FE_PROFILER_FUNC();

		for (auto layer_it = m_LayerStack.begin(); layer_it != m_LayerStack.end(); layer_it++) // auto = std::vector< std::shared_ptr< Layer > >::iterator
		{
			(*layer_it)->OnUpdate();
		}
	}

	void Application::UpdateImGui()
	{
		FE_PROFILER_FUNC();

		m_ImGuiLayer->Begin();

		for (std::shared_ptr<Layer> layer : m_LayerStack)
			layer->OnImGuiRender();

		m_ImGuiLayer->End();
	}

	void Application::TriangleTestSetup()
	{
		FE_LOG_CORE_DEBUG("TriangleTestSetup begin.");

		glCreateVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		float vertices[3 * (3+4)] = {
			-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f
		};

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		BufferLayout layout = {
			{ "a_Position", SDType::Float3 },
			{ "a_Color", SDPrimitive::Float, 4 }
		};

		uint32_t index = 0;
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.ComponentCount,
				SDPrimitiveToGLBaseType(element.Primitive),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			index++;
		}


		uint32_t indecies[3] = { 0, 1, 2 };
		
		m_IndexBuffer.reset(IndexBuffer::Create(indecies, 3));

		std::string vertexSource = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSource = R"(
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

		m_Shader.reset(Shader::Create("TestShader", vertexSource, fragmentSource));

		FE_LOG_CORE_DEBUG("TriangleTestSetup end.");
	}

	void Application::TriangleTestDraw()
	{
		FE_LOG_CORE_TRACE("TriangleTestDraw");

		m_Shader->Bind();
		glBindVertexArray(m_VertexArray);
		glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

	}
}