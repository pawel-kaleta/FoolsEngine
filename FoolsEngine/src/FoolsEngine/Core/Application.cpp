#include "FE_pch.h"

#include "FoolsEngine\Core\Application.h"
#include "FoolsEngine\Renderer\Renderer.h"



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

		Renderer::Init();

	}

	Application::~Application()
	{
		FE_PROFILER_FUNC();
	}

	void Application::OnEvent(Event& event)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_TRACE("Application::OnEvent");
		FE_LOG_CORE_TRACE(event.GetEventType());
		FE_LOG_CORE_TRACE(event.GetName());
		FE_LOG_CORE_TRACE(event.GetCategoryFlags());

		EventDispacher dispacher(event);
		//dispacher.Dispach<WindowCloseEvent>(std::bind(&Application::OnWindowCloseEvent, this, std::placeholders::_1));
		dispacher.Dispach<WindowCloseEvent>(FE_BIND_EVENT_HANDLER(Application::OnWindowCloseEvent));

	}

	bool Application::OnWindowCloseEvent(WindowCloseEvent& event)
	{
		FE_PROFILER_FUNC();

		m_Running = false;
		FE_LOG_CORE_INFO("Window Close Event");

		return true;
	}


	void Application::Run()
	{
		FE_PROFILER_FUNC();

		while (m_Running)
		{
			Time::TimePoint now = Time::Now();
			m_LastFrameTimeStep = Time::TimeStep(m_LastFrameTimePoint, now);
			m_LastFrameTimePoint = now;

			FE_LOG_CORE_TRACE("m_LastFrameTimeStep: {0}", m_LastFrameTimeStep.GetSeconds());
			FE_LOG_CORE_TRACE("m_LastFrameTimePoint: {0}", m_LastFrameTimePoint.GetTime());

			UpdateLayers();
			UpdateImGui();

			m_MainEventDispacher.DispachEvents(m_LayerStack);
			m_Window->OnUpdate();
		}


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

	

}