#pragma once

#include "FoolsEngine\Core\Core.h"
#include "FoolsEngine\Events\Event.h"
#include "FoolsEngine\Events\MainEventDispacher.h"
#include "FoolsEngine\Core\Window.h"
#include "FoolsEngine\Core\Layer.h"
#include "FoolsEngine\Core\LayerStack.h"
#include "FoolsEngine\ImGui\ImGuiLayer.h"

#include "FoolsEngine\Renderer\OrtographicCamera.h"
#include "FoolsEngine\Renderer\APIAbstraction\Shader.h"
#include "FoolsEngine\Renderer\APIAbstraction\Buffers.h"
#include "FoolsEngine\Renderer\APIAbstraction\VertexArray.h"

#include "Time.h"

namespace fe
{
	class ApplicationLayer : public Layer
	{
	public:
		ApplicationLayer(std::function<void(std::shared_ptr<Events::Event>)> EventCallback)
			: Layer("ApplicationLayer"), m_Callback(EventCallback)
		{
			FE_PROFILER_FUNC();
		}

		void OnEvent(std::shared_ptr<Events::Event> event) override { m_Callback(event); };

	private:
		std::function<void(std::shared_ptr<Events::Event>)> m_Callback;
	};

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		inline Time::TimeStep GetLastFrameTimeStep() { return m_LastFrameTimeStep; }


	protected:
		void PushInnerLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PushInnerLayer(layer);	layer->OnAttach(); }
		void PushOuterLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PushOuterLayer(layer);	layer->OnAttach(); }
		void PopInnerLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PopInnerLayer(layer);	layer->OnDetach(); }
		void PopOuterLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PopOuterLayer(layer);	layer->OnDetach(); }

	private:
		void UpdateLayers();
		void UpdateImGui();
		void OnEvent(std::shared_ptr<Events::Event> event);
		void OnWindowCloseEvent(std::shared_ptr<Events::WindowCloseEvent> event);
		void OnKeyPressedEvent(std::shared_ptr<Events::KeyPressedEvent> event);

		std::unique_ptr<Window> m_Window;
		MainEventDispacher m_MainEventDispacher;
		LayerStack m_LayerStack;
		std::shared_ptr<ApplicationLayer> m_AppLayer;
		std::shared_ptr<ImGuiLayer> m_ImGuiLayer;
		bool m_Running = true;

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
		inline Time::TimeStep FrameStep() {	return Application::Get().GetLastFrameTimeStep(); }
	}
}