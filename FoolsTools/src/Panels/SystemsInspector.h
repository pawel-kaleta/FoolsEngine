#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class SystemsInspector
	{
	public:
		SystemsInspector() = default;
		SystemsInspector(const Ref<Scene>& scene)
			: m_Scene(scene) {}

		void SetScene(const Ref<Scene>& scene);
		void OnImGuiRender();
	private:
		Ref<Scene> m_Scene;

		System* m_SystemToRemove = nullptr;

		void DrawSystemWidget(System* system);
		void AddSystemPopupMenu();
	};
}