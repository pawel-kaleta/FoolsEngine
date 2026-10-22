#include "Application.h"

#include "Time.h"
#include "Project.h"

#include "FoolsEngine/Foundation/Context/Context.h"
#include "FoolsEngine/Foundation/Context/GlobalContext.h"
#include "FoolsEngine/Foundation/Containers/Xar.h"
#include "FoolsEngine/Foundation/Allocators/Allocator.h"

#include "FoolsEngine/Platform/FileDialogs.h"
#include "FoolsEngine/Platform/Events/Event.h"
#include "FoolsEngine/Platform/Win32/Win32Window.h"
					 
#include "FoolsEngine/Renderer/5 - Integration/Renderer.h"
					 
#include "FoolsEngine/Scene/ComponentTypesRegistry.h"
#include "FoolsEngine/Scene/GameplayWorld/Actor/BehaviorsRegistry.h"
#include "FoolsEngine/Scene/GameplayWorld/System/SystemTypesRegistry.h"
					 
#include "FoolsEngine/Assets/AssetManager.h"
#include "FoolsEngine/Assets/AssetTypesRegistry.h"
#include "FoolsEngine/Assets/Loaders/LoadersRegistry.h"
#include "FoolsEngine/Assets/Serialization/AssetsRegistrySerialization.h"


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
			: Layer(String("ApplicationLayer"_FE)), m_Callback(EventCallback) {
			FE_PROFILER_FUNC();
		}

		void OnEvent(Ref<Events::Event> event) override { m_Callback(event); };

		void Shutdown() { FE_PROFILER_FUNC(); }

		std::function<void(Ref<Events::Event>)> m_Callback;
	};

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
	}


	void Application::Startup()
	{
		FE_PROFILER_FUNC();

		Context::Log::ScopeCore scope_log("App"_FE);

		{
			FE_PROFILER_SCOPE("Window");
			Context::Log::ScopeCore scope("Window"_FE);

			GlobalContext::Window::Base->mEventCallback = std::bind(&MainEventDispacher::ReceiveEvent, &m_MainEventDispacher, std::placeholders::_1);
			Context::Log::Core.Info("Callback set"_FE);
		}

		{
			FE_PROFILER_SCOPE("Renderer");
			Context::Log::ScopeCore scope("Renderer"_FE);

			Renderer::Startup();
			Context::Log::Core.Info("Renderer initialized"_FE);
		}
		
		// Types Registries
		{
			FE_PROFILER_SCOPE("Types Registries");
			Context::Log::ScopeCore scope("Types Registries"_FE);

			ComponentTypesRegistry::Init();
			ComponentTypesRegistry::RegisterComponents();
			Context::Log::Core.Info("Componets registry created"_FE);

			BehaviorsRegistry::Init();
			BehaviorsRegistry::RegisterBehaviors();
			Context::Log::Core.Info("Behaviors registry created"_FE);
		
			SystemTypesRegistry::Init();
			SystemTypesRegistry::RegisterSystems();
			Context::Log::Core.Info("Systems registry created"_FE);

			AssetTypesRegistry::Init();
			AssetTypesRegistry::RegisterAssetTypes();
			Context::Log::Core.Info("AssetTypes registry created"_FE);

			LoadersRegistry::Init();
			LoadersRegistry::RegisterLoaders();
			Context::Log::Core.Info("Loaders registry created"_FE);

			Context::Log::Core.Info("Types Registries created"_FE);
		}

		// Asset manager
		{
			FE_PROFILER_SCOPE("Asset manager");
			Context::Log::ScopeCore scope("Asset manager"_FE);

			auto asset_manager = Context::Alloc::Permanent.Allocate<AssetManager>();
			new (asset_manager) AssetManager();

			Context::Log::Core.Info("Asset manager created"_FE);
		}

		// Layers Stack
		{
			FE_PROFILER_SCOPE("Layer Stack");
			Context::Log::ScopeCore scope("Layer Stack"_FE);

			auto layer_stack = Context::Alloc::Permanent.Allocate<LayerStack>();
			new (layer_stack) LayerStack();

			Context::Log::Core.Info("Layer Stack created"_FE);
		}

		// Core layers
		{
			FE_PROFILER_SCOPE("Core layers");
			Context::Log::ScopeCore scope("Core layers"_FE);

			m_AppLayer = CreateRef<ApplicationLayer>(std::bind(&Application::OnEvent, this, std::placeholders::_1));
			LayerStack::sInstance->PushOuterLayer(m_AppLayer);
			Context::Log::Core.Info("Application layer created"_FE);

			m_ImGuiLayer = CreateRef<ImGuiLayer>();
			m_ImGuiLayer->Startup();
			LayerStack::sInstance->PushOuterLayer(m_ImGuiLayer);
			Context::Log::Core.Info("ImGui layer created"_FE);


			Context::Log::Core.Info("Core layers created"_FE);
		}

		// Client Application
		{
			FE_PROFILER_SCOPE("Client app startup");
			Context::Log::ScopeCore scope("Client app"_FE);

			ClientAppStartup();

			Context::Log::Core.Info("Client app created"_FE);
		}
	}

	void Application::Run()
	{
		{
			Context::Log::ScopeCore scope("Project loading"_FE, LoggingLevel::Info);
			Context::Log::Core.Info("Project loading start"_FE);

			m_ImGuiLayer->Begin();
			while (ClientAppProjectInit() && m_Running)
			{
				m_ImGuiLayer->End();
				m_MainEventDispacher.DispachEvents(*LayerStack::sInstance);
				GlobalContext::Window::Base->OnUpdate();
				m_ImGuiLayer->Begin();
			}
			m_ImGuiLayer->End();
		}

		while (m_Running)
		{
#ifdef FE_INTERNAL_BUILD
			if (m_ActiveProfiler)
				if (++m_ProfilerFramesCount >= 15)
				{
					FE_PROFILER_SESSION_END();
					m_ActiveProfiler = false;
				}
			
			if (m_ActivateProfiler)
			{
				FE_PROFILER_SESSION_START("Runtime", "Logs/ProfileData_Runtime.json");
				m_ProfilerFramesCount = 0;
				m_ActiveProfiler = true;
				m_ActivateProfiler = false;
			}
#endif // FE_INTERNAL_BUILD

			FE_PROFILER_SCOPE("FRAME");

			Time::TimePoint now = Time::Now();
			Time::s_LastFrameTimeStep = Time::TimeStep(Time::s_LastFrameTimePoint, now);
			Time::s_LastFrameTimePoint = now;

			Context::Log::Core.Trace("m_LastFrameTimeStep:  {0}"_FE, Time::s_LastFrameTimeStep.GetSeconds());
			Context::Log::Core.Trace("m_LastFrameTimePoint: {0}"_FE, Time::s_LastFrameTimePoint.GetTime());

			m_FrameCount++;

			auto& l_stack = *LayerStack::sInstance;

			m_MainEventDispacher.DispachEvents(l_stack);

			if (!m_Minimized)
			{
				// Layers Update
				{
					FE_PROFILER_SCOPE("Layers Update");

					for (auto layer_it = l_stack.begin(); layer_it != l_stack.end(); layer_it++) // auto = std::vector< Ref< Layer > >::iterator
					{
						(*layer_it)->OnUpdate();
					}
				}

				// ImGUI Update
				if (m_ImGuiLayer->m_Attached)
				{
					FE_PROFILER_SCOPE("ImGUI Update");

					m_ImGuiLayer->Begin();

					for (auto layer_it = l_stack.begin(); layer_it != l_stack.end(); layer_it++)
						(*layer_it)->OnImGuiRender();

					m_ImGuiLayer->End();
				}
			}
			GlobalContext::Window::Base->OnUpdate();
		}

#ifdef FE_INTERNAL_BUILD
		if (m_ActiveProfiler && !m_Running)
		{
			FE_PROFILER_SESSION_END();
			m_ActiveProfiler = false;
		}
#endif // FE_INTERNAL_BUILD
	}

	void Application::ShutDown()
	{
		FE_PROFILER_FUNC();

		// Client Application
		{
			FE_PROFILER_SCOPE("Client app shutdown");
			ClientAppShutdown();
		}

		// Layers detaching
		{
			FE_PROFILER_SCOPE("layers detaching");

			auto& l_stack = *LayerStack::sInstance;
			for (auto layer_it = l_stack.begin(); layer_it != l_stack.end(); layer_it++) // auto = std::vector< Ref< Layer > >::iterator
			{
				(*layer_it)->OnDetach();
				(*layer_it).reset();
			}

			l_stack.m_Layers.clear();
		}

		// Core Layers
		{
			FE_PROFILER_SCOPE("Core layers shutdown");
			m_ImGuiLayer->Shutdown();
			m_ImGuiLayer.reset();
			m_AppLayer->Shutdown();
			m_AppLayer.reset();
		}

		// Layer Stack
		{
			FE_PROFILER_SCOPE("Layer Stack");
			//Just leting OS reclame memory
			//delete m_LayerStack;
		}

		// Asset manager
		{
			FE_PROFILER_SCOPE("Asset Manager");
			//Just leting OS reclame memory
			//delete m_AssetManager;
		}

		// Types Registries
		{
			FE_PROFILER_SCOPE("Types Registries");

			LoadersRegistry::Shutdown();
			AssetTypesRegistry::Shutdown();
			ComponentTypesRegistry::Shutdown();
			BehaviorsRegistry::Shutdown();
			SystemTypesRegistry::Shutdown();

			/*
			Just leting OS reclame memory
		
			//delete m_AssetTypesRegistry;
			delete m_ComponentTypesRegistry;
			delete m_BehaviorsRegistry;
			delete m_SystemsRegistry;
			*/
		}

		Renderer::Shutdown();

		
	}

	Application::~Application()
	{
		FE_PROFILER_FUNC();
	}

	void Application::ProjectNew(const std::filesystem::path& filepath)
	{
#ifdef FE_INTERNAL_BUILD
		FE_PROFILER_SESSION_START("ProjectCreation", "Logs/ProfileData_ProjectCreation.json");
#endif // FE_INTERNAL_BUILD

		Project::Create(filepath);
		Renderer::AcquireBaseAssets();

#ifdef FE_INTERNAL_BUILD
		FE_PROFILER_SESSION_END();
#endif // FE_INTERNAL_BUILD
	}

	void Application::ProjectLoad(const std::filesystem::path& filepath)
	{
#ifdef FE_INTERNAL_BUILD
		FE_PROFILER_SESSION_START("ProjectLoading", "Logs/ProfileData_ProjectLoading.json");
#endif // FE_INTERNAL_BUILD

		Project::Load(filepath);
		Renderer::AcquireBaseAssets();

		auto result = AssetSerializer::DeserializeRegistry();
		if (result)   AssetSerializer::LoadMetaData();

#ifdef FE_INTERNAL_BUILD
		FE_PROFILER_SESSION_END();
#endif // FE_INTERNAL_BUILD
		
		FE_CORE_ASSERT(result, "Deserialization of asset registry failed");
	}

	void Application::ProjectSave()
	{
		Project::Save();
	}

	void Application::OnEvent(Ref<Events::Event> event)
	{
		FE_PROFILER_FUNC();
		Context::Log::Core.Trace("Application::OnEvent"_FE);
		Context::Log::Core.Trace(event->GetName());

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
		Context::Log::Core.Info("Window Close Event"_FE);
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
			m_ActivateProfiler = true;
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

		Context::Log::Core.Info("Window resize (width: {0}, height: {1})"_FE, event->GetWidth(), event->GetHeight());
	}
}