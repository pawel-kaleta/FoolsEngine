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

		void		SetScene(const Ref<Scene>& scene);
		EntityID	GetSelection()					{ return m_SelectedEntityID; }
		void		SetSelection(EntityID entityID)	{ m_SelectedEntityID = entityID; }

		void OnImGuiRender();
	private:
		Ref<Scene>	m_Scene;
		EntityID	m_SelectedEntityID = NullEntityID;
		
		bool DrawEntity(EntityID entityID);
	};
}
