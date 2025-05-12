#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class SystemsInspector
	{
	public:
		SystemsInspector() = default;
		SystemsInspector(const AssetHandle<Scene>& scene)
			: m_Scene(scene) {}

		void SetScene(const AssetHandle<Scene>& scene);
		void OnImGuiRender();
	private:
		AssetHandle<Scene> m_Scene;

		System* m_SystemToRemove = nullptr;

		void DrawSystemWidget(System* system);
		void AddSystemPopupMenu();
	};
}