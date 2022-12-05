#include "FE_pch.h"

#include "FoolsEngine\Core\Application.h"
#include "FoolsEngine\Platform\Win10\Win10Window.h"
#include "FoolsEngine\Core\InputPolling.h"

#include <glad\glad.h>

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
		FE_LOG_CORE_DEBUG("This requires OpenGL profile to be 'Compatibility', not 'Core' (look for glfw initialization).");

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

		unsigned int indecies[3] = { 0, 1, 2 };
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indecies), indecies, GL_STATIC_DRAW);

		FE_LOG_CORE_DEBUG("TriangleTestSetup end.");
	}

	void Application::TriangleTestDraw()
	{
		FE_LOG_CORE_TRACE("TriangleTestDraw");

		glBindVertexArray(m_VertexArray);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

	}
}