#include "FE_pch.h"

#include "FoolsEngine\Core\Application.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "FoolsEngine\Scene\ComponentTypesRegistry.h"
#include "FoolsEngine\Debug\SceneTesting.h"

namespace fe {

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name)
		: m_Name(name)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(!s_Instance, "Application already exists!")
		s_Instance = this;

		m_Window = Window::Create(m_Name);
		m_Window->CreateRenderingContext(GDIType::OpenGL);
		m_Window->SetEventCallback(std::bind(&MainEventDispacher::ReceiveEvent, & m_MainEventDispacher, std::placeholders::_1));

		m_AppLayer = CreateRef<ApplicationLayer>(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		m_LayerStack.PushOuterLayer(m_AppLayer);

		m_ImGuiLayer = CreateRef<ImGuiLayer>();
		m_LayerStack.PushOuterLayer(m_ImGuiLayer);

		GDIType gdi = GDIType::OpenGL;

		Renderer::Init();
		Renderer::CreateAPI(gdi);
		Renderer::InitAPI(gdi);
		Renderer::SetAPI(gdi);

		ComponentTypesRegistry::GetInstance().RegisterComponents();
		BehaviorsRegistry::GetInstance().RegisterBehaviors();
		SystemsRegistry::GetInstance().RegisterSystems();

		RegisterAndLoadStuff(); //SceneTesting.h
	}

	Application::~Application()
	{
		FE_PROFILER_FUNC();
	}

	void Application::OnEvent(Ref<Events::Event> event)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_TRACE("Application::OnEvent");
		FE_LOG_CORE_TRACE(event->GetName());

		Events::EventDispacher dispacher(event);
		dispacher.Dispach<Events::WindowCloseEvent>(FE_BIND_EVENT_HANDLER(Application::OnWindowCloseEvent));
		dispacher.Dispach<Events::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(Application::OnKeyPressedEvent));
		dispacher.Dispach<Events::WindowResizeEvent>(FE_BIND_EVENT_HANDLER(Application::OnWindowResize));
	}

	void Application::OnWindowCloseEvent(Ref<Events::WindowCloseEvent> event)
	{
		FE_PROFILER_FUNC();

		m_Running = false;
		event->Owned = true;
		FE_LOG_CORE_INFO("Window Close Event");
	}

	void Application::OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event)
	{
		if (event->GetKeyCode() == InputCodes::Escape)
		{
			m_Running = false;
			event->Owned = true;
		}

#ifdef FE_INTERNAL_BUILD
		if (event->GetKeyCode() == InputCodes::P)
		{
			if (!m_ActiveProfiler)
			{
				FE_PROFILER_SESSION_START("Runtime", "Logs/ProfileData_Runtime.json");
				m_ProfilerFramesCount = 0;
				m_ActiveProfiler = true;
			}
		}
#endif // FE_INTERNAL_BUILD
	}

	void Application::OnWindowResize(Ref<Events::WindowResizeEvent> event)
	{
		FE_PROFILER_FUNC();

		event->Handled = true;
		
		if (event->GetWidth() == 0 || event->GetHeight() == 0)
		{
			m_Minimized = true;
			return;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(event->GetWidth(), event->GetHeight());
	}

	void Application::Run()
	{
#ifdef FE_INTERNAL_BUILD
		FE_PROFILER_SESSION_START("Runtime", "Logs/ProfileData_Runtime.json");
		m_ProfilerFramesCount = 0;
		m_ActiveProfiler = true;
#endif // FE_INTERNAL_BUILD

		FE_PROFILER_FUNC();

		while (m_Running)
		{
			FE_PROFILER_SCOPE("FRAME");

			Time::TimePoint now = Time::Now();
			m_LastFrameTimeStep = Time::TimeStep(m_LastFrameTimePoint, now);
			m_LastFrameTimePoint = now;

			FE_LOG_CORE_TRACE("m_LastFrameTimeStep: {0}", m_LastFrameTimeStep.GetSeconds());
			FE_LOG_CORE_TRACE("m_LastFrameTimePoint: {0}", m_LastFrameTimePoint.GetTime());

			m_FrameCount++;

			m_MainEventDispacher.DispachEvents(m_LayerStack);

			if (!m_Minimized)
			{
				UpdateLayers();
				UpdateImGui();
			}
			m_Window->OnUpdate();

#ifdef FE_INTERNAL_BUILD
			if (m_ActiveProfiler)
				if (++m_ProfilerFramesCount >= 30)
				{
					FE_PROFILER_SESSION_END();
					m_ActiveProfiler = false;
				}
#endif // FE_INTERNAL_BUILD
		}
	}

	void Application::UpdateLayers()
	{
		FE_PROFILER_FUNC();

		for (auto layer_it = m_LayerStack.begin(); layer_it != m_LayerStack.end(); layer_it++) // auto = std::vector< Ref< Layer > >::iterator
		{
			(*layer_it)->OnUpdate();
		}
	}

	void Application::UpdateImGui()
	{
		FE_PROFILER_FUNC();

		m_ImGuiLayer->Begin();

		for (auto layer_it = m_LayerStack.begin(); layer_it != m_LayerStack.end(); layer_it++)
			(*layer_it)->OnImGuiRender();

		m_ImGuiLayer->End();
	}
}