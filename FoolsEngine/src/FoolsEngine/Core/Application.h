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
		std::string WorkingDirectory;
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

		static void				Close()					{ Get().m_Running = false; }
		static Window&			GetWindow()				{ return *(Get().m_Window); }
		//static Time::TimeStep	GetLastFrameTimeStep()	{ return Get().m_LastFrameTimeStep; }
		static ImGuiLayer*		GetImguiLayer()			{ return Get().m_ImGuiLayer.get(); }
		static uint32_t			GetFrameCount()			{ return Get().m_FrameCount; }

		
	protected:
		virtual void ClientAppStartup() {};
		virtual void ClientAppShutdown() {};
		virtual bool ClientAppProjectInit() = 0;

		void PushInnerLayer(Ref<Layer> layer) { m_LayerStack.PushInnerLayer(layer);	}
		void PushOuterLayer(Ref<Layer> layer) { m_LayerStack.PushOuterLayer(layer); }
		void PopInnerLayer(Ref<Layer> layer)  { m_LayerStack.PopInnerLayer(layer); }
		void PopOuterLayer(Ref<Layer> layer)  { m_LayerStack.PopOuterLayer(layer); }

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
		LayerStack			m_LayerStack;

		Scope<Window>			m_Window;
		Ref<ApplicationLayer>	m_AppLayer;
		Ref<ImGuiLayer>			m_ImGuiLayer;

		ComponentTypesRegistry*	m_ComponentTypesRegistry;
		BehaviorsRegistry*		m_BehaviorsRegistry;
		SystemsRegistry*		m_SystemsRegistry;
		AssetTypesRegistry*     m_AssetTypesRegistry;

		AssetManager*	m_AssetManager;

		bool		m_Running		= true;
		bool		m_Minimized		= false;
		uint32_t	m_FrameCount	= 1;

		//Time::TimePoint	m_LastFrameTimePoint;
		//Time::TimeStep	m_LastFrameTimeStep;

#ifdef FE_INTERNAL_BUILD
		bool		m_ActiveProfiler		= false;
		uint16_t	m_ProfilerFramesCount	= 0;
#endif // FE_INTERNAL_BUILD

	};

	// To be defined in FoolsEngine application (game)
	Application* CreateApplication(const ApplicationCommandLineArgs& args);

	//namespace Time
	//{
	//	extern inline float DeltaTime() { return Application::GetLastFrameTimeStep().GetSeconds(); }
	//}
}