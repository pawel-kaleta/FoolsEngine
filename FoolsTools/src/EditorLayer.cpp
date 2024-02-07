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

		m_Scene = CreateRef<Scene>();
		SetSceneContext(m_Scene);
	}

	void EditorLayer::OnUpdate()
	{
		FE_PROFILER_FUNC();
		FE_LOG_TRACE("EditorLayer::OnUpdate()");

		switch (m_EditorState)
		{
		case EditorState::Edit: 
			break;
		case EditorState::Play:
			m_Scene->SimulationUpdate();
			break;
		case EditorState::Pause:
			break;
		}

		m_Scene->PostFrameUpdate();

		m_Viewports.EditViewport.UpdateCamera();

		m_Viewports.EditViewport.RenderScene();
		m_Viewports.PlayViewport.RenderScene();
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

			m_Viewports.EditViewport.OnImGuiRender();
			m_Viewports.PlayViewport.OnImGuiRender();

			GetSelection();
		}
		ImGui::End();
	}

	void EditorLayer::RenderToolbar()
	{
		m_Toolbar.SetEditorState(m_EditorState);
		m_Toolbar.OnImGuiRender();
		ToolbarButton clickedButton = m_Toolbar.GetClickedButton();

		switch (clickedButton)
		{
		case ToolbarButton::None:
			break;
		case ToolbarButton::Play:
			if (m_EditorState == EditorState::Edit)
				OnScenePlayStart();
			if (m_EditorState == EditorState::Pause)
				OnScenePlayResume();
			break;
		case ToolbarButton::Pause:
			OnScenePlayPause();
			break;
		case ToolbarButton::Stop:
			OnScenePlayStop();
			break;
		}
	}

	void EditorLayer::RenderPanels()
	{
		FE_PROFILER_FUNC();

		SetSelectionContext();

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
				m_Viewports.EditViewport.GetCameraController().RenderWidget();
			}
		}
		ImGui::End();
	}

	void EditorLayer::RenderMainMenu()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();

				if (m_EditorState != EditorState::Edit)
					ImGui::BeginDisabled();
				{
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
				}
				if (m_EditorState != EditorState::Edit)
					ImGui::EndDisabled();

				ImGui::Separator();
				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::MenuItem("Spawn Test Scene Objects"))
					TestSceneSetup(m_Scene);

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
			if (m_EditorState != EditorState::Edit)
				m_EditorState  = EditorState::Edit;

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
		m_SelectedEntityID = NullEntityID;

		m_Panels.WorldHierarchyPanel.SetScene(scene);
		m_Panels.EntityInspector.SetScene(scene);
		m_Panels.ActorInspector.SetScene(scene);
		m_Panels.SystemsInspector.SetScene(scene);

		m_Viewports.EditViewport.SetScene(scene);
		m_Viewports.PlayViewport.SetScene(scene);
	}

	void EditorLayer::SetSelectionContext()
	{
		m_Panels.WorldHierarchyPanel.SetSelection(m_SelectedEntityID);
		m_Panels.EntityInspector.OpenEntity(m_SelectedEntityID);
		m_Panels.ActorInspector.OpenActor(m_SelectedEntityID);

		m_Viewports.EditViewport.SetSelection(m_SelectedEntityID);
	}

	void EditorLayer::GetSelection()
	{
		EntityID newSelectionID;
		bool isNewSelection = false;

		newSelectionID = m_Panels.WorldHierarchyPanel.GetSelectionRequest();
		isNewSelection |= (newSelectionID != m_SelectedEntityID);

		newSelectionID = m_Viewports.EditViewport.GetSelectionRequest();
		isNewSelection |= (newSelectionID != m_SelectedEntityID);

		newSelectionID = m_Panels.ActorInspector.GetSelectionRequest();
		isNewSelection |= (newSelectionID != m_SelectedEntityID);

		if (isNewSelection)
			m_SelectedEntityID = newSelectionID;
	}

	void EditorLayer::OnScenePlayStart()
	{
		if (!m_Scene->GetGameplayWorld()->GetEntityWithPrimaryCamera())
			FE_LOG_CORE_ERROR("No primary camera in the scene, rendering editors view");


		m_SceneBackup = CreateRef<Scene>();

		std::string sceneData = SceneSerializerYAML::Serialize(m_Scene);

		// TO DO: binary serialization
		if (SceneSerializerYAML::Deserialize(m_SceneBackup, sceneData))
		{
			m_EditorState = EditorState::Play;

			m_SceneBackup->SetFilepath(m_Scene->GetFilepath());
			m_SceneBackup->SetName(m_Scene->GetName());
		}
		else
		{
			FE_LOG_CORE_ERROR("Deserialization of scene failed");
		}
	}

	void EditorLayer::OnScenePlayPause()
	{
		m_EditorState = EditorState::Pause;
	}

	void EditorLayer::OnScenePlayResume()
	{
		m_EditorState = EditorState::Play;
	}

	void EditorLayer::OnScenePlayStop()
	{
		m_EditorState = EditorState::Edit;

		m_Scene = m_SceneBackup;
		SetSceneContext(m_Scene);
	}

	void EditorLayer::OnEvent(Ref<Events::Event> event)
	{
		FE_LOG_TRACE("{0}", event);

		Events::EventDispacher dispacher(event);
		dispacher.Dispach<Events::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(EditorLayer::OnKeyPressedEvent));

		if (event->Handled || !event->Owned)
			return;

		if (!Application::Get().GetImguiLayer()->IsBlocking())
			m_Viewports.EditViewport.OnEvent(event);
	}

	void EditorLayer::OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event)
	{
		if (event->GetRepeatCount() == 0)
		{
			bool control = InputPolling::IsKeyPressed(InputCodes::LeftControl) || InputPolling::IsKeyPressed(InputCodes::RightControl);
			bool shift = InputPolling::IsKeyPressed(InputCodes::LeftShift) || InputPolling::IsKeyPressed(InputCodes::RightShift);

			if (control)
			{
				switch (event->GetKeyCode())
				{
				case InputCodes::N:
					NewScene();
					event->Handle();
					return;
				case InputCodes::O:
					OpenScene();
					event->Handle();
					return;
				case InputCodes::S:
					if (m_EditorState != EditorState::Edit)
						return;

					if (shift || m_Scene->GetFilepath().empty())
						SaveSceneAs(m_Scene);
					else
						SaveScene(m_Scene);

					event->Handle();
					return;
				}
			}
		}
	}
}