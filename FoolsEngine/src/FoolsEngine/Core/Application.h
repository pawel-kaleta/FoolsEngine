#pragma once

#include "FoolsEngine\Core\Core.h"
#include "FoolsEngine\Events\MainEventDispacher.h"
#include "FoolsEngine\Core\Window.h"
#include "FoolsEngine\Core\Layer.h"
#include "FoolsEngine\Core\LayerStack.h"
#include "FoolsEngine\ImGui\ImGuiLayer.h"

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
	class AssetManager;
	class ApplicationLayer;
	class Project;

	class Application
	{
	public:
				 Application(const ApplicationSpecification& appSpecification);
		virtual ~Application();

		static void			Close()			{ Get().m_Running = false; }
		static Window&		GetWindow()		{ return *(Get().m_Window); }
		static ImGuiLayer*	GetImguiLayer()	{ return Get().m_ImGuiLayer.get(); }
		static uint32_t		GetFrameCount()	{ return Get().m_FrameCount; }

		
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

		static Application& Get() { return *s_Instance; }

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

		std::string			m_Name;
		MainEventDispacher	m_MainEventDispacher;
		LayerStack*			m_LayerStack = nullptr;

		Scope<Window>			m_Window;
		Ref<ApplicationLayer>	m_AppLayer;
		Ref<ImGuiLayer>			m_ImGuiLayer;

		ComponentTypesRegistry*	m_ComponentTypesRegistry = nullptr;
		BehaviorsRegistry*		m_BehaviorsRegistry = nullptr;
		SystemsRegistry*		m_SystemsRegistry = nullptr;
		AssetTypesRegistry*		m_AssetTypesRegistry = nullptr;

		AssetManager* m_AssetManager = nullptr;

		uint32_t	m_FrameCount	= 1;
		bool		m_Running		= true;
		bool		m_Minimized		= false;

#ifdef FE_INTERNAL_BUILD
		uint16_t	m_ProfilerFramesCount	= 0;
		bool		m_ActiveProfiler		= false;
		bool		m_ActivateProfiler		= true;
#endif // FE_INTERNAL_BUILD

	};

	// To be defined in FoolsEngine application (game)
	Application* CreateApplication(const ApplicationCommandLineArgs& args);
}