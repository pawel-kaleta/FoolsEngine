#include "FE_pch.h"
#include "Application.h"

#include "Time.h"
#include "Project.h"

#include "FoolsEngine/Foundation/Memory/Xar.h"
#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Memory/Scratchpad.h"

#include "FoolsEngine/Platform/FileDialogs.h"
#include "FoolsEngine/Platform/Events/Event.h"
					 
#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/7 - Integration/Renderer.h"
					 
#include "FoolsEngine/Scene/ComponentTypesRegistry.h"
#include "FoolsEngine/Scene/GameplayWorld/Actor/BehaviorsRegistry.h"
#include "FoolsEngine/Scene/GameplayWorld/System/SystemsRegistry.h"
					 
#include "FoolsEngine/Assets/AssetManager.h"
#include "FoolsEngine/Assets/AssetTypesRegistry.h"
#include "FoolsEngine/Assets/Loaders/LoadersRegistry.h"
#include "FoolsEngine/Assets/Serialization/AssetsRegistrySerialization.h"

namespace fe
{
	ApplicationSpecification* s_ApplicationSpecification = nullptr; //TO DO: get rid of this

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

		void Shutdown() { FE_PROFILER_FUNC(); }

		std::function<void(Ref<Events::Event>)> m_Callback;
	};

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& appSpecification)
		: m_Name(appSpecification.Name)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		s_ApplicationSpecification = new ApplicationSpecification;
		*s_ApplicationSpecification = appSpecification;
	}

	void Application::Startup()
	{
		FE_PROFILER_FUNC();

		// Allocators
		{
			FE_PROFILER_SCOPE("Allocators");
			Scratchpad::Init();
			Description::Library::Init();
		}

		// Rendering, Window and Platform Layer
		{
			{
				// Platform layer
			}

			{
				// Default:
				//  - Graphics API
				//  - Render Device
				//  - Window
				//  - Render Context
				//  - Swapchain
			}

			FE_PROFILER_SCOPE("Rendering, Window and Platform Layer");

			m_Window = Window::Create(s_ApplicationSpecification->WindowAttributes);

			m_Window->SetEventCallback(std::bind(&MainEventDispacher::ReceiveEvent, &m_MainEventDispacher, std::placeholders::_1));
			m_Window->CreateRenderingContext();

			GAPIType GAPI = m_Window->GetGAPIType();
			Renderer::Startup();
			Renderer::CreateAPI(GAPI);
			Renderer::InitAPI(GAPI);
			Renderer::SetAPI(GAPI);
		}
		
		// Types Registries
		{
			FE_PROFILER_SCOPE("Types Registries");

			m_ComponentTypesRegistry = new ComponentTypesRegistry();
			m_ComponentTypesRegistry->RegisterComponents();

			m_BehaviorsRegistry = new BehaviorsRegistry();
			m_BehaviorsRegistry->RegisterBehaviors();
		
			m_SystemsRegistry = new SystemsRegistry();
			m_SystemsRegistry->RegisterSystems();

			m_AssetTypesRegistry = new AssetTypesRegistry();
			m_AssetTypesRegistry->RegisterAssetTypes();

			m_LoadersRegistry = new LoadersRegistry();
			m_LoadersRegistry->RegisterLoaders();
		}

		// Asset manager
		{
			FE_PROFILER_SCOPE("Asset manager");
			m_AssetManager = new AssetManager();
		}

		// Layers Stack
		{
			FE_PROFILER_SCOPE("Layer Stack");
			m_LayerStack = new LayerStack();
		}

		// Core layers
		{
			FE_PROFILER_SCOPE("Core layers");
			m_AppLayer = CreateRef<ApplicationLayer>(std::bind(&Application::OnEvent, this, std::placeholders::_1));
			m_LayerStack->PushOuterLayer(m_AppLayer);

			m_ImGuiLayer = CreateRef<ImGuiLayer>();
			m_ImGuiLayer->Startup();
			m_LayerStack->PushOuterLayer(m_ImGuiLayer);
		}

		// Client Application
		{
			FE_PROFILER_SCOPE("Client app startup");
			ClientAppStartup();
		}
	}

	void Application::Run()
	{
		m_ImGuiLayer->Begin();
		while (ClientAppProjectInit() && m_Running)
		{
			m_ImGuiLayer->End();
			m_MainEventDispacher.DispachEvents(*m_LayerStack);
			m_Window->OnUpdate();
			m_ImGuiLayer->Begin();
		}
		m_ImGuiLayer->End();

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

			FE_LOG_CORE_TRACE("m_LastFrameTimeStep:  {0}", Time::s_LastFrameTimeStep.GetSeconds());
			FE_LOG_CORE_TRACE("m_LastFrameTimePoint: {0}", Time::s_LastFrameTimePoint.GetTime());

			m_FrameCount++;

			m_MainEventDispacher.DispachEvents(*m_LayerStack);

			if (!m_Minimized)
			{
				// Layers Update
				{
					FE_PROFILER_SCOPE("Layers Update");

					for (auto layer_it = m_LayerStack->begin(); layer_it != m_LayerStack->end(); layer_it++) // auto = std::vector< Ref< Layer > >::iterator
					{
						(*layer_it)->OnUpdate();
					}
				}

				// ImGUI Update
				if (m_ImGuiLayer->m_Attached)
				{
					FE_PROFILER_SCOPE("ImGUI Update");

					m_ImGuiLayer->Begin();

					for (auto layer_it = m_LayerStack->begin(); layer_it != m_LayerStack->end(); layer_it++)
						(*layer_it)->OnImGuiRender();

					m_ImGuiLayer->End();
				}
			}
			m_Window->OnUpdate();
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
			for (auto layer_it = m_LayerStack->begin(); layer_it != m_LayerStack->end(); layer_it++) // auto = std::vector< Ref< Layer > >::iterator
			{
				(*layer_it)->OnDetach();
				(*layer_it).reset();
			}

			m_LayerStack->m_Layers.clear();
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

			m_LoadersRegistry->Shutdown();
			m_AssetTypesRegistry->Shutdown();
			m_ComponentTypesRegistry->Shutdown();
			m_BehaviorsRegistry->Shutdown();
			m_SystemsRegistry->Shutdown();

			/*
			Just leting OS reclame memory
		
			//delete m_AssetTypesRegistry;
			delete m_ComponentTypesRegistry;
			delete m_BehaviorsRegistry;
			delete m_SystemsRegistry;
			*/
		}

		// Rendering, Window and Platform Layer
		{
			FE_PROFILER_SCOPE("Rendering, Window and Platform Layer");
			Renderer::Shutdown();
			
			delete m_Window; // abstraction, safer to destroy
		}

		// Allocators
		{
			FE_PROFILER_SCOPE("Allocators");
			Scratchpad::Shutdown();
		}
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

		FE_LOG_CORE_INFO("Window resize (width: {0}, height: {1})", event->GetWidth(), event->GetHeight());
	}
}