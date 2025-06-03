#include "EditorLayer.h"
#include "SceneTesting.h"

#include "AssetImport\FileHandler.h"

#include <string>
#include <filesystem>

namespace fe
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
		FE_PROFILER_FUNC();

		RegisterAndLoadStuff(); //SceneTesting.h
	}

	void EditorLayer::OnAttach()
	{
		FE_PROFILER_FUNC();
		FE_LOG_INFO("EditorLayer::OnAttach()");

		NewScene();
	}

	void EditorLayer::OnUpdate()
	{
		FE_PROFILER_FUNC();
		FE_LOG_TRACE("EditorLayer::OnUpdate()");

		{
			auto scene_user = m_Scene.Use();
			switch (m_EditorState)
			{
			case EditorState::Edit: 
				break;
			case EditorState::Play:
				scene_user.SimulationUpdate();
				break;
			case EditorState::Pause:
				break;
			}

			scene_user.PostFrameUpdate();
		}//scope to kill scene_user

		m_Viewports.EditViewport.OnUpdate();

		AssetManager::EvaluateAndReload();
		m_Viewports.EditViewport.RenderScene();
		m_Viewports.PlayViewport.RenderScene();
	}

	void EditorLayer::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

		m_DockSpace.BeginDockSpace();

		RenderMainMenu();
		RenderToolbar();
		RenderPanels();

		m_Viewports.PlayViewport.OnImGuiRender();
		m_Viewports.EditViewport.OnImGuiRender();

		AssetImportModal::OnImGuiRender();

		GetSelection();
		
		m_DockSpace.EndDockSpace();
	}

	void EditorLayer::RenderToolbar()
	{
		FE_PROFILER_FUNC();

		m_Toolbar.SetEditorState(m_EditorState);
		m_Toolbar.OnImGuiRender();

		{
			FE_PROFILER_SCOPE("Handle Toolbar Input");

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
	}

	void EditorLayer::RenderPanels()
	{
		FE_PROFILER_FUNC();

		SetSelectionContext();

		m_Panels.ContentBrowser.OnImGuiRender();
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
		FE_PROFILER_FUNC();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N")) NewScene();
				if (ImGui::MenuItem("Open...",   "Ctrl+O")) OpenScene();

				if (m_EditorState != EditorState::Edit) ImGui::BeginDisabled();

				if (ImGui::MenuItem("Save",       "Ctrl+S"))       SaveScene();
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) SaveSceneAs();

				if (m_EditorState != EditorState::Edit) ImGui::EndDisabled();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) Application::Close();

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::MenuItem("Spawn Test Scene Objects"))
				{
					//RegisterAndLoadStuff(); //EditorLayer::EditorLayer() already calls  //SceneTesting.h 
					TestSceneSetup(m_Scene.Observe());
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::NewScene()
	{
		FE_PROFILER_FUNC();

		std::filesystem::path filepath = FileDialogs::SaveFile(".\\assets\\scenes\\scene.fescene", "FoolsEngine Scene (*.fescene)\0 * .fescene\0");

		m_Scene = AssetHandle<Scene>(AssetManager::CreateProjectAsset<Scene>(filepath), LoadingPriority_Critical);
		m_Scene.Use().Initialize();

		SetSceneContext(m_Scene);
	}

	void EditorLayer::OpenScene()
	{
		FE_PROFILER_FUNC();

		std::filesystem::path filepath = FileDialogs::OpenFile("FoolsEngine Scene (*.fescene)\0*.fescene\0");
		if (filepath.empty())
			return;

		auto& current_scene_filepath = m_Scene.Observe().GetFilepath();
		if (current_scene_filepath == filepath)
			return;
		
		AssetHandle<Scene> newScene = AssetHandle<Scene>(AssetManager::GetAssetFromFilepath(filepath));
		bool new_scene_opened = false;
		if (newScene.IsValid())
		{
			auto newScene_user = newScene.Use();
			newScene_user.Initialize();

			new_scene_opened = SceneSerializerYAML::DeserializeFromFile(newScene_user);
		}
		else
		{
			FE_LOG_CORE_ERROR("Adding scenes to project not implemented");
		}

		if (new_scene_opened)
		{
			m_EditorState  = EditorState::Edit;

			m_Scene = newScene;
			SetSceneContext(m_Scene);
		}
		else
		{
			FE_LOG_CORE_ERROR("Deserialization of a scene failed: {0}", filepath.string());
		}
	}

	void EditorLayer::SaveScene()
	{
		FE_PROFILER_FUNC();

		SceneSerializerYAML::SerializeToFile(m_Scene.Observe());
	}

	void EditorLayer::SaveSceneAs()
	{
		FE_PROFILER_FUNC();

		FE_LOG_CORE_ERROR("Adding scenes to project not implemented");

		std::filesystem::path filepath = FileDialogs::SaveFile(".\\assets\\scenes\\scene.fescene", "FoolsEngine Scene (*.fescene)\0 * .fescene\0");
		
		if (filepath.empty()) return;
	
		AssetHandle<Scene>(AssetManager::CreateProjectAsset<Scene>(filepath));

		FE_CORE_ASSERT(false, "Not implemented");
		
		SaveScene();
	}

	void EditorLayer::SetSceneContext(const AssetHandle<Scene>& scene)
	{
		FE_PROFILER_FUNC();

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
		FE_PROFILER_FUNC();

		{
			if (!m_Scene.Use().GetCoreComponent().GameplayWorld->GetRegistry().valid(m_SelectedEntityID))
				m_SelectedEntityID = NullEntityID;
		}

		m_Panels.WorldHierarchyPanel.SetSelection(m_SelectedEntityID);
		m_Panels.EntityInspector.OpenEntity(m_SelectedEntityID);
		m_Panels.ActorInspector.OpenActor(m_SelectedEntityID);

		m_Viewports.EditViewport.SetSelection(m_SelectedEntityID);
	}

	void EditorLayer::GetSelection()
	{
		FE_PROFILER_FUNC();

		EntityID newSelection = NullEntityID;
		bool isNewSelection = false;

		EntityID newSelectionRequest_1 = m_Panels.WorldHierarchyPanel.GetSelectionRequest();
		if (newSelectionRequest_1 != m_SelectedEntityID)
		{
			newSelection = newSelectionRequest_1;
			isNewSelection = true;
		}

		EntityID newSelectionRequest_2 = m_Viewports.EditViewport.GetSelectionRequest();
		if (newSelectionRequest_2 != NullEntityID)
		{
			newSelection = newSelectionRequest_2;
			isNewSelection = true;
		}

		EntityID newSelectionRequest_3 = m_Panels.ActorInspector.GetSelectionRequest();
		if (newSelectionRequest_3 != NullEntityID)
		{
			newSelection = newSelectionRequest_3;
			isNewSelection = true;
		}

		if (isNewSelection)
			m_SelectedEntityID = newSelection;
	}

	void EditorLayer::OnScenePlayStart()
	{
		FE_PROFILER_FUNC();

		auto scene_observer = m_Scene.Observe();

		if (!scene_observer.GetCoreComponent().GameplayWorld->GetEntityWithPrimaryCamera())
			FE_LOG_CORE_ERROR("No primary camera in the scene, rendering editors view");

		m_SceneBackup = SceneSerializerYAML::SerializeToString(scene_observer);
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
		FE_PROFILER_FUNC();

		m_EditorState = EditorState::Edit;

		auto scene_user = m_Scene.Use();
		scene_user.Release();
		scene_user.Initialize();
		bool deserialization_result = SceneSerializerYAML::DeserializeFromString(scene_user, m_SceneBackup);
		FE_CORE_ASSERT(deserialization_result, "Scene recovery after play stop failed.");
		m_SceneBackup.clear();
	}

	void EditorLayer::OnEvent(Ref<Events::Event> event)
	{
		FE_PROFILER_FUNC();

		FE_LOG_TRACE("{0}", event);

		Events::EventDispacher dispacher(event);
		dispacher.Dispach<Events::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(EditorLayer::OnKeyPressedEvent));

		if (event->Handled || event->Owned)
			return;

		if (!Application::GetImguiLayer()->IsBlocking())
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

					if (shift || m_Scene.Observe().AllOf<ACFilepath>())
						SaveSceneAs();
					else
						SaveScene();

					event->Handle();
					return;
				}
			}
		}
	}
}