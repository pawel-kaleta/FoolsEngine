#pragma once

#include "FoolsEngine\Core\Core.h"
#include "FoolsEngine\Events\Event.h"
#include "FoolsEngine\Events\MainEventDispacher.h"
#include "FoolsEngine\Core\Window.h"
#include "FoolsEngine\Core\Layer.h"
#include "FoolsEngine\Core\LayerStack.h"
#include "FoolsEngine\ImGui\ImGuiLayer.h"

#include "FoolsEngine\Renderer\OrthographicCamera.h"
#include "FoolsEngine\Renderer\APIAbstraction\Shader.h"
#include "FoolsEngine\Renderer\APIAbstraction\Buffers.h"
#include "FoolsEngine\Renderer\APIAbstraction\VertexArray.h"

#include "Time.h"

int main(int argc, char** argv);

namespace fe
{
	class ApplicationLayer : public Layer
	{
	public:
		ApplicationLayer(std::function<void(Ref<Events::Event>)> EventCallback)
			: Layer("ApplicationLayer"), m_Callback(EventCallback)
		{
			FE_PROFILER_FUNC();
		}

		void OnEvent(Ref<Events::Event> event) override { m_Callback(event); };

	private:
		std::function<void(Ref<Events::Event>)> m_Callback;
	};

	class Application
	{
	public:
		Application();
		virtual ~Application();


		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		inline Time::TimeStep GetLastFrameTimeStep() { return m_LastFrameTimeStep; }


	protected:
		void PushInnerLayer(Ref<Layer> layer) { m_LayerStack.PushInnerLayer(layer);	layer->OnAttach(); }
		void PushOuterLayer(Ref<Layer> layer) { m_LayerStack.PushOuterLayer(layer);	layer->OnAttach(); }
		void PopInnerLayer(Ref<Layer> layer) { m_LayerStack.PopInnerLayer(layer);	layer->OnDetach(); }
		void PopOuterLayer(Ref<Layer> layer) { m_LayerStack.PopOuterLayer(layer);	layer->OnDetach(); }

	private:
		friend int ::main(int argc, char** argv);
		void Run();
		void UpdateLayers();
		void UpdateImGui();
		void OnEvent(Ref<Events::Event> event);
		void OnWindowCloseEvent(Ref<Events::WindowCloseEvent> event);
		void OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event);
		void OnWindowResize(Ref<Events::WindowResizeEvent> event);

		Scope<Window> m_Window;
		MainEventDispacher m_MainEventDispacher;
		LayerStack m_LayerStack;
		Ref<ApplicationLayer> m_AppLayer;
		Ref<ImGuiLayer> m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;

		Time::TimePoint m_LastFrameTimePoint;
		Time::TimeStep m_LastFrameTimeStep;

		static Application* s_Instance;

#ifdef FE_INTERNAL_BUILD
		bool m_ActiveProfiler = false;
		uint16_t m_ProfilerFramesCount = 0;
#endif // FE_INTERNAL_BUILD

	};

	// To be defined in FoolsEngine application (game)
	Application* CreateApplication();

	namespace Time
	{
		inline float DeltaTime() { return Application::Get().GetLastFrameTimeStep().GetSeconds(); }
	}
}