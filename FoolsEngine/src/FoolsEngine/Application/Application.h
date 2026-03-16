#pragma once

#include "Layer.h"
#include "LayerStack.h"
#include "ImGui/ImGuiLayer.h"

#include "FoolsEngine/Foundation/Utils/Core.h"

#include "FoolsEngine/Platform/Window.h"
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

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			FE_CORE_ASSERT(index < Count, "ApplicationCommandLineArgs index out of scope");
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "FoolsEngine Application";
		WindowAttributes WindowAttributes;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class ComponentTypesRegistry;
	class BehaviorsRegistry;
	class SystemsRegistry;
	class AssetTypesRegistry;
	class LoadersRegistry;
	class AssetManager;
	class ApplicationLayer;
	class Project;

	class Application
	{
	public:
				 Application(const ApplicationSpecification& appSpecification);
		virtual ~Application();

		static Application& Get() { return *s_Instance; }
		static void Close() { s_Instance->m_Running = false; }

		std::string			m_Name;
		MainEventDispacher	m_MainEventDispacher;
		LayerStack*			m_LayerStack;

		Window*					m_Window;
		Ref<ApplicationLayer>	m_AppLayer;
		Ref<ImGuiLayer>			m_ImGuiLayer;

		ComponentTypesRegistry*	m_ComponentTypesRegistry;
		BehaviorsRegistry*		m_BehaviorsRegistry;
		SystemsRegistry*		m_SystemsRegistry;
		AssetTypesRegistry*		m_AssetTypesRegistry;
		LoadersRegistry*		m_LoadersRegistry;

		AssetManager* m_AssetManager;

		uint32_t	m_FrameCount	= 1;
		bool		m_Running		= true;
		bool		m_Minimized		= false;
		
	protected:
		virtual void ClientAppStartup() {};
		virtual void ClientAppShutdown() {};
		virtual bool ClientAppProjectInit() = 0;

		void PushInnerLayer(Ref<Layer> layer) { m_LayerStack->PushInnerLayer(layer);	}
		void PushOuterLayer(Ref<Layer> layer) { m_LayerStack->PushOuterLayer(layer); }
		void PopInnerLayer(Ref<Layer> layer)  { m_LayerStack->PopInnerLayer(layer); }
		void PopOuterLayer(Ref<Layer> layer)  { m_LayerStack->PopOuterLayer(layer); }

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
	Application* CreateApplication(const ApplicationCommandLineArgs& args);
}