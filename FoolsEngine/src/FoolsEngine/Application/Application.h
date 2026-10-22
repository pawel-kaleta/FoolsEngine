#pragma once

#include "Layer.h"
#include "LayerStack.h"
#include "ImGui/ImGuiLayer.h"

#include "FoolsEngine/Foundation/Utils/Core.h"
#include "FoolsEngine/Foundation/Containers/String.h"

#include "FoolsEngine/Platform/Window.h"
#include "FoolsEngine/Platform/PlatformBase.h"
#include "FoolsEngine/Platform/Events/MainEventDispacher.h"

int main(int argc, char** argv);
namespace fe
{
	namespace Events
	{
		class Event;
		class WindowCloseEvent;
		class KeyPressedEvent;
		class WindowResizeEvent;
	}

	class ComponentTypesRegistry;
	class BehaviorsRegistry;
	class SystemTypesRegistry;
	class AssetTypesRegistry;
	class LoadersRegistry;
	class AssetManager;
	class ApplicationLayer;
	class Project;

	class Application
	{
	public:
				 Application();
		virtual ~Application();

		static Application& Get() { return *s_Instance; }
		static void Close() { s_Instance->m_Running = false; }

		String				m_Name;
		MainEventDispacher	m_MainEventDispacher;

		Ref<ApplicationLayer>	m_AppLayer;
		Ref<ImGuiLayer>			m_ImGuiLayer;

		UInt	m_FrameCount	= 1;
		bool	m_Running		= true;
		bool	m_Minimized		= false;
		
	protected:
		virtual void ClientAppStartup() {};
		virtual void ClientAppShutdown() {};
		virtual bool ClientAppProjectInit() = 0;

		void PushInnerLayer(Ref<Layer> layer) { LayerStack::sInstance->PushInnerLayer(layer);	}
		void PushOuterLayer(Ref<Layer> layer) { LayerStack::sInstance->PushOuterLayer(layer); }
		void PopInnerLayer(Ref<Layer> layer)  { LayerStack::sInstance->PopInnerLayer(layer); }
		void PopOuterLayer(Ref<Layer> layer)  { LayerStack::sInstance->PopOuterLayer(layer); }

		void ProjectNew(const std::filesystem::path& filepath);
		void ProjectLoad(const std::filesystem::path& filepath);
		void ProjectSave();

	private:
		static Application* s_Instance;

		friend int ::main(int argc, char** argv);
		void Startup();
		void Run();
		void ShutDown();

		void OnEvent(Ref<Events::Event> event);
		void OnWindowCloseEvent(Ref<Events::WindowCloseEvent> event);
		void OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event);
		void OnWindowResize(Ref<Events::WindowResizeEvent> event);


#ifdef FE_INTERNAL_BUILD
		uint16_t	m_ProfilerFramesCount	= 0;
		bool		m_ActiveProfiler		= false;
		bool		m_ActivateProfiler		= true;
#endif // FE_INTERNAL_BUILD
	};

	// To be defined in FoolsEngine application (game)
	Application* CreateApplication();
}