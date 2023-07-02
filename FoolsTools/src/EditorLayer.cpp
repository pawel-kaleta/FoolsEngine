#include "EditorLayer.h"

namespace fe
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
	{
	}

	struct TargetBehaviourScript : NativeScript
	{
		float MoveSpeed = 0.5f;

		void OnUpdate() override
		{
			auto transform = GetTransformHandle();
			auto newTransform = transform.Global();
			auto& position = newTransform.Position;
			auto& rotation = newTransform.Rotation;
			auto& scale = newTransform.Scale;


			float moveDistance = Time::DeltaTime() * MoveSpeed;
			float rotSpeed = Time::DeltaTime() * 80.0f;
			float scaleSpeed = Time::DeltaTime() * 0.2f;

				 if (InputPolling::IsKeyPressed(InputCodes::KP1))	scale -= scaleSpeed;
			else if (InputPolling::IsKeyPressed(InputCodes::KP3))	scale += scaleSpeed;

			     if (InputPolling::IsKeyPressed(InputCodes::KP4))	rotation.z -= rotSpeed;
			else if (InputPolling::IsKeyPressed(InputCodes::KP6))	rotation.z += rotSpeed;

				 if (InputPolling::IsKeyPressed(InputCodes::Right))	position.x += moveDistance;
			else if (InputPolling::IsKeyPressed(InputCodes::Left))	position.x -= moveDistance;
				 if (InputPolling::IsKeyPressed(InputCodes::Up))	position.y += moveDistance;
			else if (InputPolling::IsKeyPressed(InputCodes::Down))	position.y -= moveDistance;

			transform = newTransform;
		}
	};

	void EditorLayer::OnAttach()
	{
		FE_PROFILER_FUNC();

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_Scene = CreateRef<Scene>();
		m_SceneHierarchyPanel.SetScene(m_Scene);

		Set tintedTextureQuad = m_Scene->CreateSet();
		{
			auto& quad = tintedTextureQuad.Emplace<Renderer2D::CQuad>();
			quad.Texture = TextureLibrary::Get("Default_Texture");
			quad.Color = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);
			quad.Layer = Renderer2D::Layer::L_2;
			quad.TextureTilingFactor = 3;
			quad.Transparency = false;
			 
			Transform transform;
			transform.Scale = glm::vec3(0.6f, 0.4f, 1.0f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
			tintedTextureQuad.GetTransformHandle() = transform;
		}

		m_ColorQuad = m_Scene->CreateSet();
		{
			auto& quad2 = m_ColorQuad.Emplace<Renderer2D::CQuad>();
			quad2.Color = glm::vec4(0.9f, 0.2f, 0.9f, 0.8f);
			quad2.Layer = Renderer2D::Layer::L_1;

			Transform transform;
			transform.Position = glm::vec3(-0.1f, -0.1f, 0.0f);
			transform.Scale = glm::vec3(0.3f, 0.2f, 1.0f);
			m_ColorQuad.GetTransformHandle() = transform;
		}

		Set target = m_Scene->CreateSet(RootID, "Target");
		{
			TextureLibrary::Add(Texture2D::Create("assets/textures/Texture_with_Transparency.png"));

			auto& quad = target.Emplace<Renderer2D::CQuad>();
			quad.Layer = Renderer2D::Layer::L1;
			quad.Texture = TextureLibrary::Get("Texture_with_Transparency");

			Transform transform;
			transform.Position = glm::vec3(0.0f, 0.0f, 0.0f);
			transform.Scale = glm::vec3(0.3f, 0.3f, 1.0f);
			target.GetTransformHandle() = transform;

			target.AddScript<TargetBehaviourScript>();

			auto tags = target.GetTagsHandle().Local();
			tags.Common.Add(CommonTags::Player);
			target.GetTagsHandle().SetLocal(tags);
		}

		Set targetChild_1 = m_Scene->CreateSet(target, "TargetChild");
		{
			auto& quad = targetChild_1.Emplace<Renderer2D::CQuad>();
			quad.Layer = Renderer2D::Layer::L1;
			quad.Texture = TextureLibrary::Get("Texture_with_Transparency");
			quad.Color = { 1.0f, 1.0f, 1.0f, 0.5f };

			Transform transform;
			transform.Position = glm::vec3(0.8f, 0.8f, 0.8f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
			transform.Scale = glm::vec3(0.5f, 0.5f, 1.0f);
			targetChild_1.GetTransformHandle().SetLocal(transform);
		}
	}

	void EditorLayer::OnUpdate()
	{
		FE_PROFILER_FUNC();
		FE_LOG_TRACE("EditorLayer::OnUpdate()");

		if (m_VieportFocus)
		{
			m_Scene->UpdateScripts();
			m_CameraController.OnUpdate();
			m_Scene->DestroyFlaggedSets();
			m_Scene->GetHierarchy().MakeGlobalTransformsCurrent();
		}

		m_Framebuffer->Bind();
		Renderer2D::RenderScene(*m_Scene, m_CameraController.GetCamera(), m_CameraController.GetTransform());
		m_Framebuffer->Unbind();
	}

	

	void EditorLayer::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

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

			m_SceneHierarchyPanel.OnImGuiRender();

			ImGui::Begin("Settings");
			{
				auto& colref = m_ColorQuad.Get<Renderer2D::CQuad>().Color;
				ImGui::ColorEdit4("CQuad color", (float*)&colref);
			}
			ImGui::End();

			ImGui::Begin("RenderStats");
			{
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

				auto& vidgetSize = ImGui::GetContentRegionAvail();
				glm::vec2 newViewPortSize = { vidgetSize.x, vidgetSize.y };
				
				if (m_ViewportSize != newViewPortSize)
				{
					m_Framebuffer->Resize((uint32_t)newViewPortSize.x, (uint32_t)newViewPortSize.y);
					m_ViewportSize = newViewPortSize;
					m_CameraController.Resize(newViewPortSize.x, newViewPortSize.y);
				}

				auto fbID = m_Framebuffer->GetColorAttachmentID();
				ImGui::Image((void*)(uint64_t)fbID, vidgetSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
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