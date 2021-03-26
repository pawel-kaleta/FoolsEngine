#pragma once
#include "FE_pch.h"
#include "FoolsEngine/Core/Core.h"
#include "FoolsEngine/Events/Event.h"
#include "FoolsEngine/Core/Window.h"
#include "FoolsEngine/Core/Layer.h"
#include "FoolsEngine/Core/LayerStack.h"

namespace fe {

	class MainEventDispacher
	{
	public:
		MainEventDispacher() { FE_PROFILER_FUNC(); }

		void ReceiveEvent(std::shared_ptr<Event> event);
		void DispachEvents(LayerStack& layerStack);
	private:
		std::vector<std::shared_ptr<Event>> m_eventsQueue;
	};

	class ApplicationLayer : public Layer
	{
	public:
		ApplicationLayer(std::function<void(std::shared_ptr<Event>)> EventCallback)
			: Layer("ApplicationLayer"), m_Callback(EventCallback)
		{
			FE_PROFILER_FUNC();
		}

		void OnEvent(std::shared_ptr<Event> event) override { m_Callback(event); };
	private:
		std::function<void(std::shared_ptr<Event>)> m_Callback;
	};

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	protected:
		void PushInnerLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PushInnerLayer(layer); }
		void PushOuterLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PushOuterLayer(layer); }
		void PopInnerLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PopInnerLayer(layer); }
		void PopOuterLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PopOuterLayer(layer); }

	private:
		void UpdateLayers();
		void OnEvent(std::shared_ptr<Event> event);
		void OnWindowCloseEvent(std::shared_ptr<Event> event);

		std::unique_ptr<Window> m_Window;
		MainEventDispacher m_MainEventDispacher;
		LayerStack m_LayerStack;
		std::shared_ptr<ApplicationLayer> m_AppLayer;
		bool m_Running = true;
	};

	// To be defined in FoolsEngine application (game)
	Application* CreateApplication();
}