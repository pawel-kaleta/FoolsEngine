#include "FE_pch.h"

#include "FoolsEngine\Core\Application.h"
#include "FoolsEngine\Renderer\Renderer.h"



namespace fe {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(!s_Instance, "Application already exists!")
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(std::bind(&MainEventDispacher::ReceiveEvent, & m_MainEventDispacher, std::placeholders::_1));

		m_AppLayer = std::make_shared<ApplicationLayer>(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		m_LayerStack.PushOuterLayer(m_AppLayer);

		m_ImGuiLayer = std::make_shared<ImGuiLayer>();
		m_LayerStack.PushOuterLayer(m_ImGuiLayer);

		Renderer::Init();

		TriangleTestSetup();
	}

	Application::~Application()
	{
		FE_PROFILER_FUNC();
	}

	void Application::OnEvent(std::shared_ptr<Event> event)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_TRACE("Application::OnEvent");
		FE_LOG_CORE_TRACE(event->GetEventType());
		FE_LOG_CORE_TRACE(event->GetName());
		FE_LOG_CORE_TRACE(event->GetCategoryFlags());

		EventDispacher dispacher(event);
		dispacher.Dispach<WindowCloseEvent>(std::bind(&Application::OnWindowCloseEvent, this, std::placeholders::_1));
	}

	bool Application::OnWindowCloseEvent(std::shared_ptr<WindowCloseEvent> event)
	{
		FE_PROFILER_FUNC();

		m_Running = false;
		FE_LOG_CORE_INFO("Window Close Event");

		return true;
	}


	void Application::Run()
	{
		FE_PROFILER_FUNC();

		while (m_Running)
		{
			RenderCommands::Clear();
			RenderCommands::SetClearColor({ 0.1, 0.1, 0.1, 1 });

			Renderer::BeginScene();
			{
				FE_LOG_CORE_TRACE("TriangleTestDraw");

				m_Shader->Bind();
				Renderer::Submit(m_VertexArray);
			}
			Renderer::EndScene();

			UpdateLayers();
			UpdateImGui();

			m_MainEventDispacher.DispachEvents(m_LayerStack);
			m_Window->OnUpdate();
		}
		
		Log::SetClientLoggingLevel(spdlog::level::trace);
		Log::SetCoreLoggingLevel(spdlog::level::trace);
	}

	void Application::UpdateLayers()
	{
		FE_PROFILER_FUNC();

		for (auto layer_it = m_LayerStack.begin(); layer_it != m_LayerStack.end(); layer_it++) // auto = std::vector< std::shared_ptr< Layer > >::iterator
		{
			(*layer_it)->OnUpdate();
		}
	}

	void Application::UpdateImGui()
	{
		FE_PROFILER_FUNC();

		m_ImGuiLayer->Begin();

		for (std::shared_ptr<Layer> layer : m_LayerStack)
			layer->OnImGuiRender();

		m_ImGuiLayer->End();
	}

	void Application::TriangleTestSetup()
	{
		FE_LOG_CORE_DEBUG("TriangleTestSetup begin.");

		m_VertexArray.reset(VertexArray::Create());
		m_VertexArray->Bind();

		float vertices[3 * (3+4)] = {
			-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f
		};

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		m_VertexBuffer->SetLayout({
			{ SDType::Float3, "a_Position" },
			{ "a_Color", SDPrimitive::Float, 4 }
		});
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indecies[3] = { 0, 1, 2 };
		m_IndexBuffer.reset(IndexBuffer::Create(indecies, 3));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		std::string vertexSource = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSource = R"(
			#version 330 core
			
			layout(location = 0) out vec4 o_color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				o_color = vec4(v_Position + 0.5, 1.0);
				o_color = v_Color;
			}
		)";

		m_Shader.reset(Shader::Create("TestShader", vertexSource, fragmentSource));

		FE_LOG_CORE_DEBUG("TriangleTestSetup end.");
	}

}