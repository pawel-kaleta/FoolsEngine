#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class WorldHierarchyPanel
	{
	public:
		WorldHierarchyPanel() = default;
		WorldHierarchyPanel(const Ref<Scene>& scene)
			: m_Scene(scene) {}

		void     SetScene(const Ref<Scene>& scene);
		void     SetSelection(EntityID entityID) { m_SelectedEntityID = entityID; }
		EntityID GetSelectionRequest() { return m_EntityIDSelectionRequest; }

		void OnImGuiRender();
	private:
		Ref<Scene> m_Scene;
		EntityID   m_SelectedEntityID = NullEntityID;
		EntityID   m_EntityIDSelectionRequest = NullEntityID;
		
		bool DrawEntity(EntityID entityID);
	};
}
