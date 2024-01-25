#include "EditorLayer.h"
#include "SceneTesting.h"

#include <string>
#include <filesystem>

namespace fe
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
		RegisterAndLoadStuff(); //SceneTesting.h
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
		SetSceneContext(m_Scene);

		m_IconPlay  = Texture2D::Create("resources/PlayButton.png");
		m_IconStop  = Texture2D::Create("resources/StopButton.png");
		m_IconPause = Texture2D::Create("resources/PauseButton.png");

		// initializing ImGui windows
		if (0)
		{
			ImGui::Begin("Dockspace");

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspaceID);
			}

			RenderToolbar();
			RenderPanels();

			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Vieport");
			ImGui::PopStyleVar();

			m_VieportFocus = ImGui::IsWindowFocused();
			m_VieportHover = ImGui::IsWindowHovered();

			bool test = ImGui::IsItemEdited();
			Application::Get().GetImguiLayer()->BlockEvents(!(m_VieportFocus || m_VieportHover));

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

			ImGui::End();
		}
	}

	void EditorLayer::OnUpdate()
	{
		FE_PROFILER_FUNC();
		FE_LOG_TRACE("EditorLayer::OnUpdate()");

		static const Camera* viewportCamera = &m_CameraController->GetCamera();
		static Transform viewportCameraTransform;

		switch (m_SceneState)
		{
		case SceneState::Edit:
			if (! Application::Get().GetImguiLayer()->IsBlocking())
				m_CameraController->OnUpdate();
			viewportCamera = &m_CameraController->GetCamera();
			viewportCameraTransform = m_CameraController->GetTransform();
			break;
		case SceneState::Play:
			m_Scene->SimulationUpdate();
		case SceneState::Pause:
			Entity cameraEntity = m_Scene->GetGameplayWorld()->GetEntityWithPrimaryCamera();
			if (cameraEntity)
			{
				auto& camera = cameraEntity.Get<CCamera>();
				viewportCamera = &camera.Camera;
				viewportCameraTransform = cameraEntity.GetTransformHandle().GetGlobal();
				viewportCameraTransform.Scale = { 1.f,1.f,1.f };
				viewportCameraTransform = viewportCameraTransform + camera.Offset;
			}
			else
			{
				viewportCamera = &m_CameraController->GetCamera();
				viewportCameraTransform = m_CameraController->GetTransform();
			}
		}

		m_Scene->PostFrameUpdate();

		m_Framebuffer->Bind();
		Renderer2D::RenderScene(*m_Scene, *viewportCamera, viewportCameraTransform);
		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

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
		static bool dockspaceOpen = true;
		//ImGui::Begin("Dockspace", 0, windowFlags);
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

			RenderMainMenu();
			RenderToolbar();

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

		bool test = ImGui::IsItemEdited();
		Application::Get().GetImguiLayer()->BlockEvents(!(m_VieportFocus || m_VieportHover));

		auto vidgetSize = ImGui::GetContentRegionAvail();
		glm::vec2 newViewPortSize = { vidgetSize.x, vidgetSize.y }; // most likely simple cast possible, but still different data types from different librarys

		if (m_ViewportSize != newViewPortSize)
		{
			// there is a bug in ImGui that is causing GetContentRegionAvail() to report wrong values in first frame
			// this is a workaround that prevents creation of framebuffer with 0 hight or with
			if (newViewPortSize.x == 0 || newViewPortSize.y == 0)
				newViewPortSize = { 1, 1 };

			m_Framebuffer->Resize((uint32_t)newViewPortSize.x, (uint32_t)newViewPortSize.y);
			m_ViewportSize = newViewPortSize;
			m_CameraController->Resize(newViewPortSize.x, newViewPortSize.y);
		}

		auto fbID = m_Framebuffer->GetColorAttachmentID();
		ImGui::Image((void*)(uint64_t)fbID, vidgetSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
	}

	void EditorLayer::RenderToolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;
		
		switch (m_SceneState)
		{
		case SceneState::Edit:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconPlay->GetID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				OnScenePlay();
			break;
		case SceneState::Play:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconPause->GetID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				OnScenePause();
			ImGui::SameLine();
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconStop->GetID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				OnSceneStop();
			break;
		case SceneState::Pause:
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconPlay->GetID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				OnScenePlay();
			ImGui::SameLine();
			if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconStop->GetID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
				OnSceneStop();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::RenderPanels()
	{
		FE_PROFILER_FUNC();


		SetSelectionContext(m_SelectedEntityID);
		  
		m_Panels.WorldHierarchyPanel.OnImGuiRender();
		m_Panels.ActorInspector.OnImGuiRender();
		m_Panels.EntityInspector.OnImGuiRender();
		m_Panels.SystemsInspector.OnImGuiRender();

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

		GetSelection();
	}

	void EditorLayer::RenderMainMenu()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				{
					NewScene();
				}
				if (ImGui::MenuItem("Open...", "Ctrl+O"))
				{
					OpenScene();
				}
				if (m_SceneState != SceneState::Edit)
					ImGui::BeginDisabled();
				if (!m_Scene->GetFilepath().empty())
				{
					if (ImGui::MenuItem("Save", "Ctrl+S"))
					{
						SaveScene(m_Scene);
					}
				}
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				{
					SaveSceneAs(m_Scene);
				}
				if (m_SceneState != SceneState::Edit)
					ImGui::EndDisabled();
				ImGui::Separator();
				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::MenuItem("Spawn Test Scene Objects"))
				{
					TestSceneSetup(m_Scene);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::NewScene()
	{
		m_Scene = CreateRef<Scene>();
		SetSceneContext(m_Scene);
	}

	void EditorLayer::OpenScene()
	{
		std::filesystem::path filepath = FileDialogs::OpenFile("FoolsEngine Scene (*.fescene.yaml)\0*.fescene.yaml\0");
		if (filepath.empty())
			return;

		Ref<Scene> newScene = CreateRef<Scene>();
		newScene->SetFilepath(filepath);
		newScene->SetName(filepath.filename().string());

		if (SceneSerializerYAML::Deserialize(newScene, filepath))
		{
			if (m_SceneState != SceneState::Edit)
				m_SceneState = SceneState::Edit;

			m_Scene = newScene;
			SetSceneContext(m_Scene);
		}
		else
		{
			FE_LOG_CORE_ERROR("Deserialization of a scene failed: {0}", filepath.string());
		}
	}

	void EditorLayer::SaveScene(const Ref<Scene>& scene)
	{
		SceneSerializerYAML::Serialize(m_Scene, scene->GetFilepath());
	}

	void EditorLayer::SaveSceneAs(const Ref<Scene>& scene)
	{
		std::filesystem::path filepath = FileDialogs::SaveFile(".\\assets\\scenes\\scene.fescene.yaml", "FoolsEngine Scene (*.fescene.yaml)\0 * .fescene.yaml\0");
		if (!filepath.empty())
		{
			scene->SetFilepath(filepath);
			SaveScene(scene);
		}
	}

	void EditorLayer::SetSceneContext(const Ref<Scene>& scene)
	{
		SetSelectionContext(NullEntityID);

		m_Panels.WorldHierarchyPanel.SetScene(scene);
		m_Panels.EntityInspector.SetScene(scene);
		m_Panels.ActorInspector.SetScene(scene);
		m_Panels.SystemsInspector.SetScene(scene);
	}

	void EditorLayer::SetSelectionContext(EntityID entityID)
	{
		m_SelectedEntityID = entityID;

		m_Panels.WorldHierarchyPanel.SetSelection(entityID);
		m_Panels.EntityInspector.OpenEntity(entityID);
		m_Panels.ActorInspector.OpenActor(entityID);
	}

	void EditorLayer::GetSelection()
	{
		m_SelectedEntityID = m_Panels.WorldHierarchyPanel.GetSelection();

		EntityID newSelection = m_Panels.ActorInspector.GetSelection();
		if (newSelection != NullEntityID)
			m_SelectedEntityID = newSelection;
	}

	void EditorLayer::OnScenePlay()
	{
		if (!m_Scene->GetGameplayWorld()->GetEntityWithPrimaryCamera())
			FE_LOG_CORE_ERROR("No primary camera in the scene, rendering editors view");
	
		m_SceneState = SceneState::Play;
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;
	}

	void EditorLayer::OnScenePause()
	{
		m_SceneState = SceneState::Pause;
	}

	void EditorLayer::OnEvent(Ref<Events::Event> event)
	{
		FE_LOG_TRACE("{0}", event);

		m_CameraController->OnEvent(event);

		Events::EventDispacher dispacher(event);
		dispacher.Dispach<Events::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(EditorLayer::OnKeyPressedEvent));
	}

	void EditorLayer::OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event)
	{
		if (event->GetRepeatCount() == 0)
		{
			bool control = InputPolling::IsKeyPressed(InputCodes::LeftControl) || InputPolling::IsKeyPressed(InputCodes::RightControl);
			bool shift   = InputPolling::IsKeyPressed(InputCodes::LeftShift  ) || InputPolling::IsKeyPressed(InputCodes::RightShift  );

			switch (event->GetKeyCode())
			{
			case InputCodes::N:
				if (control)
					NewScene();
				break;
			case InputCodes::O:
				if (control)
					OpenScene();
				break;
			case InputCodes::S:
				if (control)
				{
					if (shift || m_Scene->GetFilepath().empty())
						SaveSceneAs(m_Scene);
					else
						SaveScene(m_Scene);
				}
				break;
			}
		}
	}
}