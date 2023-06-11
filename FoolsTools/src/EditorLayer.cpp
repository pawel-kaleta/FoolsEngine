#include "EditorLayer.h"

namespace fe
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true)
	{
	}

	void EditorLayer::OnAttach()
	{
		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_Scene = CreateScope<Scene>();

		Set tintedTextureQuad = m_Scene->CreateSet();
		{
			auto& quad = tintedTextureQuad.Emplace<Renderer2D::Quad>();
			quad.Texture = TextureLibrary::Get("Default_Texture");
			quad.Color = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);
			quad.Layer = Renderer2D::Layer::L_2;
			quad.TextureTilingFactor = 3;
			quad.Transparency = false;

			auto& transform = tintedTextureQuad.Emplace<CTransform>();
			transform.Scale = glm::vec3(0.8f, 0.8f, 1.0f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
		}

		m_ColorQuad = m_Scene->CreateSet();
		{
			auto& quad = m_ColorQuad.Emplace<Renderer2D::Quad>();
			quad.Color = glm::vec4(0.9f, 0.2f, 0.9f, 0.8f);
			quad.Layer = Renderer2D::Layer::L_1;

			auto& transform = m_ColorQuad.Emplace<CTransform>();
			transform.Position = glm::vec3(-0.1f, -0.1f, 0.0f);
			transform.Scale = glm::vec3(0.4f, 0.4f, 1.0f);
		}

		m_Target = m_Scene->CreateSet();
		{
			TextureLibrary::Add(Texture2D::Create("assets/textures/Texture_with_Transparency.png"));

			auto& quad = m_Target.Emplace<Renderer2D::Quad>();
			quad.Layer = Renderer2D::Layer::L0;
			quad.Texture = TextureLibrary::Get("Texture_with_Transparency");

			auto& transform = m_Target.Emplace<CTransform>();
			transform.Position = glm::vec3(0.0f, 0.0f, 0.0f);
			transform.Scale = glm::vec3(0.6f, 0.6f, 1.0f);
		}
	}

	void EditorLayer::OnUpdate()
	{
		FE_PROFILER_FUNC();
		FE_LOG_TRACE("EditorLayer::OnUpdate()");

		if (m_VieportFocus)
		{
			float moveDistance = Time::DeltaTime() * m_TargetMoveSpeed;

			auto& targetPosition = m_Target.Get<CTransform>().Position;

				 if (InputPolling::IsKeyPressed(InputCodes::Right))	targetPosition.x += moveDistance;
			else if (InputPolling::IsKeyPressed(InputCodes::Left))	targetPosition.x -= moveDistance;
				 if (InputPolling::IsKeyPressed(InputCodes::Up))	targetPosition.y += moveDistance;
			else if (InputPolling::IsKeyPressed(InputCodes::Down))	targetPosition.y -= moveDistance;

			m_CameraController.OnUpdate();
		}

		m_Framebuffer->Bind();
		Renderer2D::RenderScene(*m_Scene, m_CameraController.GetCamera());
		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool constFullscreenOpt = true;
		bool fullscreenOpt = constFullscreenOpt;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_::ImGuiDockNodeFlags_None;

		// nested docking spaces of the same size bad -> no docking to window, only to dedicated dockspace
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;
		if (fullscreenOpt)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowBorderSize, 0.0f);

			windowFlags |=
				ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
			
				ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspaceFlags & ImGuiDockNodeFlags_::ImGuiDockNodeFlags_PassthruCentralNode)
			windowFlags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Dockspace Test", &dockspaceOpen, windowFlags);
		{
			ImGui::PopStyleVar();

			if (fullscreenOpt)
				ImGui::PopStyleVar(2);

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
			}

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Exit"))
						Application::Get().Close();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::Begin("Settings");
			{
				auto& colref = m_ColorQuad.Get<Renderer2D::Quad>().Color;
				ImGui::ColorEdit4("Quad color", (float*)&colref);

				ImGui::Text("Stats:");
				auto& stats = Renderer2D::GetStats();
				ImGui::Text("Draw Calls: %d", stats.DrawCalls);
				ImGui::Text("Quads: %d", stats.Quads);
				ImGui::Text("Render Time: %F", stats.RenderTime.GetMilliseconds());
				ImGui::Text("Frame Time: %F", Time::DeltaTime() * 1000);
				ImGui::Text("FPS: %F", 1.0f / Time::DeltaTime());
			}
			ImGui::End();

			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Vieport");
			{
				ImGui::PopStyleVar();

				m_VieportFocus = ImGui::IsWindowFocused();
				m_VieportHover = ImGui::IsWindowHovered();
				Application::Get().GetImguiLayer()->BlockEvents(!(m_VieportFocus && m_VieportHover));

				auto& windowSize = ImGui::GetContentRegionAvail();
				glm::vec2 newViewPortSize = { windowSize.x, windowSize.y };
				
				if (m_ViewportSize != newViewPortSize)
				{
					m_Framebuffer->Resize((uint32_t)newViewPortSize.x, (uint32_t)newViewPortSize.y);
					m_ViewportSize = newViewPortSize;
					m_CameraController.Resize(newViewPortSize.x, newViewPortSize.y);
				}

				auto fbID = m_Framebuffer->GetColorAttachmentID();
				ImGui::Image((void*)fbID, windowSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			}
			ImGui::End();

		}
		ImGui::End();	
	}

	void EditorLayer::OnEvent(Ref<Events::Event> event)
	{
		FE_LOG_TRACE("{0}", event);

		// OnImguRender() is handling viewport resizing
		if (event->GetEventType() != Events::EventType::WindowResize)
			m_CameraController.OnEvent(event);

		Events::EventDispacher dispacher(event);
		dispacher.Dispach<Events::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(EditorLayer::OnKeyPressedEvent));
	}

	void EditorLayer::OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event)
	{
	
	}

}