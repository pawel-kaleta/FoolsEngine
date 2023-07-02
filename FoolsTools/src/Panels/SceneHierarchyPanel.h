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

		void OnImGuiRender();
	private:
		Ref<Scene> m_Scene;
		SetID m_SelectedSetID = NullSetID;
		
		void DrawSet(SetID setID);
	};
}
