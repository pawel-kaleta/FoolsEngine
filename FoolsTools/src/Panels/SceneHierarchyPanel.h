#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene)
			: m_Scene(scene) {}

		void SetScene(const Ref<Scene>& scene);
		SetID GetSelection() { return m_SelectedSetID; }
		void SetSelection(SetID setID) { m_SelectedSetID = setID; }

		void OnImGuiRender();
	private:
		Ref<Scene> m_Scene;
		SetID m_SelectedSetID = NullSetID;
		
		bool DrawSet(SetID setID);
	};
}
