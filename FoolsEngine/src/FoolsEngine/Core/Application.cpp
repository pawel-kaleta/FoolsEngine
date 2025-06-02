#include "FE_pch.h"
#include "Application.h"
#include "Project.h"

#include "FoolsEngine\Platform\FileDialogs.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include "FoolsEngine\Scene\ComponentTypesRegistry.h"
#include "FoolsEngine\Scene\GameplayWorld\Actor\BehaviorsRegistry.h"
#include "FoolsEngine\Scene\GameplayWorld\System\SystemsRegistry.h"

#include "FoolsEngine\Assets\AssetTypesRegistry.h"
#include "FoolsEngine\Assets\AssetManager.h"
#include "FoolsEngine\Assets\Serializers\AssetsSerializer.h"

#include "FoolsEngine\Events\Event.h"

#include "FoolsEngine\Memory\Scratchpad.h"

#include "Time.h"

namespace fe
{
	namespace Time
	{
		extern TimePoint s_LastFrameTimePoint;
		extern TimeStep	s_LastFrameTimeStep;
	}

	class ApplicationLayer : public Layer
	{
	public:
		ApplicationLayer(std::function<void(Ref<Events::Event>)> EventCallback)
			: Layer("ApplicationLayer"), m_Callback(EventCallback) {
			FE_PROFILER_FUNC();
		}

		void OnEvent(Ref<Events::Event> event) override { m_Callback(event); };

		void Shutdown() { }
	private:
		std::function<void(Ref<Events::Event>)> m_Callback;
	};

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name, WindowAttributes attributes)
		: m_Name(name)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create(attributes);
		m_Window->SetEventCallback(std::bind(&MainEventDispacher::ReceiveEvent, & m_MainEventDispacher, std::placeholders::_1));
		m_Window->CreateRenderingContext();
	}

	void Application::Startup()
	{
		FE_PROFILER_FUNC();

		{
			Scratchpad::Init();
		}

		{
			FE_PROFILER_SCOPE("Type Registries");
			m_ComponentTypesRegistry = new ComponentTypesRegistry();
			m_ComponentTypesRegistry->RegisterComponents();

			m_BehaviorsRegistry = new BehaviorsRegistry();
			m_BehaviorsRegistry->RegisterBehaviors();
		
			m_SystemsRegistry = new SystemsRegistry();
			m_SystemsRegistry->RegisterSystems();
		}

		{
			FE_PROFILER_SCOPE("Asset Type Registrie");
			m_AssetTypesRegistry = new AssetTypesRegistry();
			m_AssetTypesRegistry->RegisterAssetTypes();
		}

		{
			FE_PROFILER_SCOPE("Asset manager");
			m_AssetManager = new AssetManager();
		}

		{
			FE_PROFILER_SCOPE("Core layers");
			m_AppLayer = CreateRef<ApplicationLayer>(std::bind(&Application::OnEvent, this, std::placeholders::_1));
			m_LayerStack.PushOuterLayer(m_AppLayer);

			m_ImGuiLayer = CreateRef<ImGuiLayer>();
			m_ImGuiLayer->Startup();
			m_LayerStack.PushOuterLayer(m_ImGuiLayer);
		}

		{
			FE_PROFILER_SCOPE("Project");

#ifdef FE_EDITOR
			std::filesystem::path filepath = FileDialogs::OpenFile("FoolsEngine Project (*.feproj)\0*.feproj\0");
#else
			std::filesystem::path filepath = "SandboxProject.feproj";
#endif
			m_Project = new Project();
			m_Project->Startup();
			Project::Load(filepath);

			AssetSerializer::DeserializeRegistry(m_Project->AssetsPath);
			AssetSerializer::LoadMetaData();
		}

		{
			FE_PROFILER_SCOPE("Rendering");
			GDIType gdi = m_Window->GetGDIType();
			Renderer::Startup();
			Renderer::CreateAPI(gdi);
			Renderer::InitAPI(gdi);
			Renderer::SetAPI(gdi);
		}

		{
			FE_PROFILER_SCOPE("Client app startup");
			ClientAppStartup();
		}
	}

	void Application::Run()
	{
		FE_PROFILER_FUNC();

#ifdef FE_INTERNAL_BUILD
		FE_PROFILER_SESSION_START("Runtime", "Logs/ProfileData_Runtime.json");
		m_ProfilerFramesCount = 0;
		m_ActiveProfiler = true;
#endif // FE_INTERNAL_BUILD

		while (m_Running)
		{
			FE_PROFILER_SCOPE("FRAME");

			Time::TimePoint now = Time::Now();
			Time::s_LastFrameTimeStep = Time::TimeStep(Time::s_LastFrameTimePoint, now);
			Time::s_LastFrameTimePoint = now;

			FE_LOG_CORE_TRACE("m_LastFrameTimeStep:  {0}", Time::s_LastFrameTimeStep.GetSeconds());
			FE_LOG_CORE_TRACE("m_LastFrameTimePoint: {0}", Time::s_LastFrameTimePoint.GetTime());

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
				if (++m_ProfilerFramesCount >= 5)
				{
					FE_PROFILER_SESSION_END();
					m_ActiveProfiler = false;
				}
#endif // FE_INTERNAL_BUILD
		}
	}

	void Application::ShutDown()
	{
		FE_PROFILER_FUNC();

		{
			FE_PROFILER_SCOPE("Client app shutdown");
			ClientAppShutdown();
		}

		{
			FE_PROFILER_SCOPE("layers detaching");
			for (auto layer_it = m_LayerStack.begin(); layer_it != m_LayerStack.end(); layer_it++) // auto = std::vector< Ref< Layer > >::iterator
			{
				(*layer_it)->OnDetach();
				(*layer_it).reset();
			}

			m_LayerStack.m_Layers.clear();
		}

		{
			FE_PROFILER_SCOPE("Core layers shutdown");
			m_ImGuiLayer->Shutdown();
			m_ImGuiLayer.reset();
			m_AppLayer->Shutdown();
			m_AppLayer.reset();
		}

		{
			FE_PROFILER_SCOPE("Rendering");
			Renderer::Shutdown();
		}

		{
			FE_PROFILER_SCOPE("Project");
			m_Project->Shutdown();
			//Just leting OS reclame memory
			//delete m_Project;
		}

		{
			FE_PROFILER_SCOPE("Asset Manager");
			m_AssetManager->Shutdown();
			//Just leting OS reclame memory
			//delete m_AssetManager;
		}

		{
			FE_PROFILER_SCOPE("Asset Types Registry");
			m_AssetTypesRegistry->Shutdown();
			//Just leting OS reclame memory
			//delete m_AssetTypesRegistry;
		}

		{
			FE_PROFILER_SCOPE("Type Registers");
			m_ComponentTypesRegistry->Shutdown();
			m_BehaviorsRegistry->Shutdown();
			m_SystemsRegistry->Shutdown();
			/*
			Just leting OS reclame memory
		
			delete m_ComponentTypesRegistry;
			delete m_BehaviorsRegistry;
			delete m_SystemsRegistry;
			*/
		}
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

		if (!m_ImGuiLayer->m_Attached)
			return;

		m_ImGuiLayer->Begin();

		for (auto layer_it = m_LayerStack.begin(); layer_it != m_LayerStack.end(); layer_it++)
			(*layer_it)->OnImGuiRender();

		m_ImGuiLayer->End();
	}
}