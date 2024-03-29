#pragma once

#include "FoolsEngine\Core\Core.h"
#include "FoolsEngine\Events\Event.h"
#include "FoolsEngine\Events\MainEventDispacher.h"
#include "FoolsEngine\Core\Window.h"
#include "FoolsEngine\Core\Layer.h"
#include "FoolsEngine\Core\LayerStack.h"
#include "FoolsEngine\ImGui\ImGuiLayer.h"

#include "Time.h"

int main(int argc, char** argv);

namespace fe
{
	class Scene;

	class ApplicationLayer : public Layer
	{
	public:
		ApplicationLayer(std::function<void(Ref<Events::Event>)> EventCallback)
			: Layer("ApplicationLayer"), m_Callback(EventCallback) { FE_PROFILER_FUNC(); }

		void OnEvent(Ref<Events::Event> event) override { m_Callback(event); };

	private:
		std::function<void(Ref<Events::Event>)> m_Callback;
	};

	class Application
	{
	public:
				 Application(const std::string& name = "Fools Engine Application", WindowAttributes attributes = WindowAttributes());
		virtual ~Application();

				void			Close()							{ m_Running = false; }
		static	Application&	Get()							{ return *s_Instance; }
				Window&			GetWindow()						{ return *m_Window; }
				Time::TimeStep	GetLastFrameTimeStep()			{ return m_LastFrameTimeStep; }
				ImGuiLayer*		GetImguiLayer()					{ return m_ImGuiLayer.get(); }
				uint32_t		GetFrameCount()					{ return m_FrameCount; }
				void			ActivateScene(Ref<Scene> scene) { m_ActiveScene = scene; }

	protected:
		void PushInnerLayer	(Ref<Layer> layer)	{ m_LayerStack.PushInnerLayer(layer);	}
		void PushOuterLayer	(Ref<Layer> layer)	{ m_LayerStack.PushOuterLayer(layer);	}
		void PopInnerLayer	(Ref<Layer> layer)	{ m_LayerStack.PopInnerLayer(layer); }
		void PopOuterLayer	(Ref<Layer> layer)	{ m_LayerStack.PopOuterLayer(layer); }

	private:
		friend int ::main(int argc, char** argv);

		void Run();
		void UpdateLayers();
		void UpdateImGui();
		void OnEvent(Ref<Events::Event> event);
		void OnWindowCloseEvent(Ref<Events::WindowCloseEvent> event);
		void OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event);
		void OnWindowResize(Ref<Events::WindowResizeEvent> event);

		std::string				m_Name;
		Scope<Window>			m_Window;
		MainEventDispacher		m_MainEventDispacher;
		LayerStack				m_LayerStack;
		Ref<ApplicationLayer>	m_AppLayer;
		Ref<ImGuiLayer>			m_ImGuiLayer;

		bool		m_Running		= true;
		bool		m_Minimized		= false;
		uint32_t	m_FrameCount	= 1;
		Ref<Scene>	m_ActiveScene;

		Time::TimePoint	m_LastFrameTimePoint;
		Time::TimeStep	m_LastFrameTimeStep;
		
		static Application* s_Instance;

#ifdef FE_INTERNAL_BUILD
		bool		m_ActiveProfiler		= false;
		uint16_t	m_ProfilerFramesCount	= 0;
#endif // FE_INTERNAL_BUILD

	};

	// To be defined in FoolsEngine application (game)
	Application* CreateApplication();

	namespace Time
	{
		inline float DeltaTime() { return Application::Get().GetLastFrameTimeStep().GetSeconds(); }
	}
}