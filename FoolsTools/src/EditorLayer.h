#pragma once

#include <FoolsEngine.h>
#include "EditorCameraController.h"
#include "Panels\WorldHierarchyPanel.h"
#include "Panels\ActorInspector.h"
#include "Panels\EntityInspector.h"
#include "Panels\SystemsInspector.h"
#include "Panels\ContentBrowser.h"

#include "Viewports\EditViewport.h"
#include "Viewports\PlayViewport.h"

#include "EditorState.h"
#include "Toolbar.h"
#include "DockSpace.h"


#include <ImGuizmo.h>


namespace fe
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		virtual void OnAttach()							override;
		virtual void OnUpdate()							override;
		virtual void OnImGuiRender()					override;
		virtual void OnEvent(Ref<Events::Event> event)	override;

	private:
		AssetHandle<Scene> m_Scene;
		std::string m_SceneBackup;

		EditorState m_EditorState = EditorState::Edit;

		DockSpace m_DockSpace;
		Toolbar m_Toolbar;

		struct Panels
		{
			WorldHierarchyPanel	WorldHierarchyPanel;
			ActorInspector		ActorInspector;
			EntityInspector		EntityInspector;
			SystemsInspector	SystemsInspector;
			ContentBrowser		ContentBrowser;
		} m_Panels;

		struct Vieports
		{
			EditViewport EditViewport;
			PlayViewport PlayViewport;
		} m_Viewports;

		EntityID m_SelectedEntityID = NullEntityID;

		void RenderPanels();
		void RenderMainMenu();
		void RenderToolbar();

		void NewScene();
		void OpenScene();
		void SaveScene();
		void SaveSceneAs();

		void SetSceneContext(const AssetHandle<Scene>& scene);
		void SetSelectionContext();
		void GetSelection();

		void OnScenePlayStart();
		void OnScenePlayPause();
		void OnScenePlayResume();
		void OnScenePlayStop();

		void OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event);
	};
}