#include "EditorLayer.h"
#include "TestScene.h"



#include <string>

namespace fe
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		FE_PROFILER_FUNC();
		FE_LOG_INFO("EditorLayer::OnAttach()");

		FramebufferData::Specification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_CameraController = CreateScope<EditorCameraController>(1280.0f, 720.0f);

		m_Scene = CreateRef<Scene>();
		m_SceneHierarchyPanel.SetScene(m_Scene);
		m_EntityInspector.SetScene(m_Scene);
		m_ActorInspector.SetScene(m_Scene);
		m_SystemsInspector.SetScene(m_Scene);

		TestSceneSetup(m_Scene.get()); //TestScene.h
	}

	void EditorLayer::OnUpdate()
	{
		FE_PROFILER_FUNC();
		FE_LOG_TRACE("EditorLayer::OnUpdate()");

		if (m_VieportFocus)
		{
			m_Scene->SimulationUpdate();

			m_CameraController->OnUpdate();
		}
		m_Scene->PostFrameUpdate();

		m_Framebuffer->Bind();
		Renderer2D::RenderScene(*m_Scene, m_CameraController->GetCamera(), m_CameraController->GetTransform());
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
		ImGui::Begin("Dockspace", &dockspaceOpen, windowFlags);
		{
			ImGui::PopStyleVar();

			if (fullscreenOpt)
				ImGui::PopStyleVar(2);

			ImGuiIO& io = ImGui::GetIO();
			ImGuiStyle& style = ImGui::GetStyle();
			float minWinSizeX = style.WindowMinSize.x;
			style.WindowMinSize.x = 300.0f;
			if (io.ConfigFlags & ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
			}
			style.WindowMinSize.x = minWinSizeX;

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Exit"))
						Application::Get().Close();
					if (ImGui::MenuItem("Serialize"))
					{
						FE_LOG_CORE_INFO("SERIALIZING");
						SceneSerializerYAML::Serialize(m_Scene, "assets/scenes/Example.fescene.yaml");
					}
					if (ImGui::MenuItem("Deserialize"))
					{
						m_Scene = CreateRef<Scene>();

						m_SceneHierarchyPanel.SetScene(m_Scene);
						m_EntityInspector.SetScene(m_Scene);
						m_ActorInspector.SetScene(m_Scene);
						m_SystemsInspector.SetScene(m_Scene);

						SceneSerializerYAML::Deserialize(m_Scene, "assets/scenes/Example.fescene.yaml");
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			RenderPanels();
			RenderViewport();
		}
		ImGui::End();	
	}

	void EditorLayer::RenderViewport()
	{
		FE_PROFILER_FUNC();

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Vieport");
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
			m_CameraController->Resize(newViewPortSize.x, newViewPortSize.y);
		}

		auto fbID = m_Framebuffer->GetColorAttachmentID();
		ImGui::Image((void*)(uint64_t)fbID, vidgetSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
	}

	void EditorLayer::RenderPanels()
	{
		FE_PROFILER_FUNC();

		m_SceneHierarchyPanel.SetSelection(m_SelectedEntityID);
		m_SceneHierarchyPanel.OnImGuiRender();
		m_SelectedEntityID = m_SceneHierarchyPanel.GetSelection();
		
		m_ActorInspector.OpenActor(m_SelectedEntityID);
		m_ActorInspector.OnImGuiRender();

		m_EntityInspector.OpenEntity(m_SelectedEntityID);
		m_EntityInspector.OnImGuiRender();

		m_SystemsInspector.OnImGuiRender();

		ImGui::Begin("RenderStats");
		{
			FE_PROFILER_SCOPE("RenderStats");

			auto& stats = Renderer2D::GetStats();
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quads: %d", stats.Quads);
			ImGui::Text("Render Time: %F", stats.RenderTime.GetMilliseconds());
			ImGui::Text("Frame Time: %F", Time::DeltaTime() * 1000);
			ImGui::Text("FPS: %F", 1.0f / Time::DeltaTime());
		}
		ImGui::End();

		ImGui::Begin("Settings");
		{
			FE_PROFILER_SCOPE("Settings");

			if (ImGui::CollapsingHeader("Editor Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				m_CameraController->RenderWidget();
			}
		}
		ImGui::End();
	}

	void EditorLayer::OnEvent(Ref<Events::Event> event)
	{
		FE_LOG_TRACE("{0}", event);

		m_CameraController->OnEvent(event);
	}

}