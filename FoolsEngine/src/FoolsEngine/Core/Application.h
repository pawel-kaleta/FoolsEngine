#pragma once

#include "FoolsEngine\Core\Core.h"
#include "FoolsEngine\Events\Event.h"
#include "FoolsEngine\Events\MainEventDispacher.h"
#include "FoolsEngine\Core\Window.h"
#include "FoolsEngine\Core\Layer.h"
#include "FoolsEngine\Core\LayerStack.h"
#include "FoolsEngine\ImGui\ImGuiLayer.h"

#include "FoolsEngine\Renderer\Shader.h"
#include "FoolsEngine\Renderer\Buffers.h"
#include "FoolsEngine\Renderer\VertexArray.h"

namespace fe
{
	class ApplicationLayer : public Layer
	{
	public:
		ApplicationLayer(std::function<void(std::shared_ptr<Event>)> EventCallback)
			: Layer("ApplicationLayer"), m_Callback(EventCallback)
		{
			FE_PROFILER_FUNC();
		}

		void OnEvent(std::shared_ptr<Event> event) override { m_Callback(event); };
		void OnImGuiRender() override
		{
			static bool show = true;
			ImGui::ShowDemoWindow(&show);
		}
	private:
		std::function<void(std::shared_ptr<Event>)> m_Callback;
	};

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		inline static Application& Get() { return *s_Instance;  }
		inline Window& GetWindow() { return *m_Window; }

	protected:
		void PushInnerLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PushInnerLayer(layer);	layer->OnAttach(); }
		void PushOuterLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PushOuterLayer(layer);	layer->OnAttach(); }
		void PopInnerLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PopInnerLayer(layer);	layer->OnDetach(); }
		void PopOuterLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PopOuterLayer(layer);	layer->OnDetach(); }

	private:
		void UpdateLayers();
		void UpdateImGui();
		void OnEvent(std::shared_ptr<Event> event);
		void OnWindowCloseEvent(std::shared_ptr<Event> event);

		void TriangleTestSetup();
		void TriangleTestDraw();
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;

		std::unique_ptr<Window> m_Window;
		MainEventDispacher m_MainEventDispacher;
		LayerStack m_LayerStack;
		std::shared_ptr<ApplicationLayer> m_AppLayer;
		std::shared_ptr<ImGuiLayer> m_ImGuiLayer;
		bool m_Running = true;

		static Application* s_Instance;
	};

	// To be defined in FoolsEngine application (game)
	Application* CreateApplication();
}