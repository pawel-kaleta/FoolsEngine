#include "EditorLayer.h"

namespace fe
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, true)
	{
		m_QuadTextureTint.Texture = TextureLibrary::Get("Default_Texture");
		m_QuadTextureTint.Color = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);
		m_QuadTextureTint.Position = glm::vec2(0.0f, -0.0f);
		m_QuadTextureTint.Size = glm::vec2(0.8f, 0.8f);
		m_QuadTextureTint.Layer = Renderer2D::Layer::L_2;
		m_QuadTextureTint.TextureTilingFactor = 3;
		m_QuadTextureTint.Transparency = false;
		m_QuadTextureTint.Rotation = 20.0f;

		m_QuadColor.Color = glm::vec4(0.9f, 0.2f, 0.9f, 0.8f);
		m_QuadColor.Position = glm::vec2(-0.1f, -0.1f);
		m_QuadColor.Size = glm::vec2(0.4f, 0.4f);
		m_QuadColor.Layer = Renderer2D::Layer::L_1;

		TextureLibrary::Add(Texture2D::Create("assets/textures/Texture_with_Transparency.png"));
		m_QuadTexture.Texture = TextureLibrary::Get("Texture_with_Transparency");
		m_QuadTexture.Position = glm::vec2(0.0f, 0.0f);
		m_QuadTexture.Size = glm::vec2(0.6f, 0.6f);
		m_QuadTexture.Layer = Renderer2D::Layer::L0;
	}

	void EditorLayer::OnAttach()
	{
		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);
	}

	void EditorLayer::OnUpdate()
	{
		FE_PROFILER_FUNC();
		FE_LOG_TRACE("EditorLayer::OnUpdate()");
		float dt = Time::DeltaTime();

		if (InputPolling::IsKeyPressed(InputCodes::Right))
		{
			m_QuadTexture.Position.x += m_QuadMoveSpeed * dt;
		}
		else if (InputPolling::IsKeyPressed(InputCodes::Left))
		{
			m_QuadTexture.Position.x -= m_QuadMoveSpeed * dt;
		}
		if (InputPolling::IsKeyPressed(InputCodes::Up))
		{
			m_QuadTexture.Position.y += m_QuadMoveSpeed * dt;
		}
		else if (InputPolling::IsKeyPressed(InputCodes::Down))
		{
			m_QuadTexture.Position.y -= m_QuadMoveSpeed * dt;
		}

		m_CameraController.OnUpdate();

		m_Framebuffer->Bind();
		Renderer2D::BeginScene(m_CameraController.GetCamera());
		{
			Renderer2D::DrawQuad(m_QuadTexture);
			Renderer2D::DrawQuad(m_QuadColor);
			Renderer2D::DrawQuad(m_QuadTextureTint);
		}
		Renderer2D::EndScene();
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
				//ImGui::ColorEdit4("Triangle color", (float*)m_Triangle.MaterialInstance->GetUniformValuePtr("u_Color"));
				ImGui::ColorEdit4("Quad color", &m_QuadColor.Color.r);

				ImGui::Text("Stats:");
				auto& stats = Renderer2D::GetStats();
				ImGui::Text("Draw Calls: %d", stats.DrawCalls);
				ImGui::Text("Quads: %d", stats.Quads);
				ImGui::Text("Render Time: %F", stats.RenderTime.GetMilliseconds());
				ImGui::Text("Frame Time: %F", Time::DeltaTime() * 1000);
				ImGui::Text("FPS: %F", 1.0f / Time::DeltaTime());
			}
			ImGui::End();

			ImGui::Begin("Vieport");
			{
				uint32_t fbID = m_Framebuffer->GetColorAttachmentID();
				ImVec2 res = { (float)m_Framebuffer->GetSpecification().Width, (float)m_Framebuffer->GetSpecification().Height };
				ImGui::Image((void*)fbID, res, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			}
			ImGui::End();

		}
		ImGui::End();	
	}

	void EditorLayer::OnEvent(Ref<Events::Event> event)
	{
		FE_LOG_TRACE("{0}", event);

		m_CameraController.OnEvent(event);

		Events::EventDispacher dispacher(event);
		dispacher.Dispach<Events::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(EditorLayer::OnKeyPressedEvent));
	}

	void EditorLayer::OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event)
	{
	
	}

}