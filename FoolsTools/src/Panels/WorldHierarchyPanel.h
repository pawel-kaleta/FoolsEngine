#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class WorldHierarchyPanel
	{
	public:
		WorldHierarchyPanel() = default;
		WorldHierarchyPanel(const AssetHandle<Scene>& scene)
			: m_Scene(scene) {}

		void     SetScene(const AssetHandle<Scene>& scene);
		void     SetSelection(EntityID entityID) { m_SelectedEntityID = entityID; }
		EntityID GetSelectionRequest() { return m_EntityIDSelectionRequest; }

		void OnImGuiRender();
	private:
		AssetHandle<Scene> m_Scene;
		EntityID   m_SelectedEntityID = NullEntityID;
		EntityID   m_EntityIDSelectionRequest = NullEntityID;
		
		bool DrawEntity(const AssetObserver<Scene>& sceneObserver, EntityID entityID);
	};
}
