#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class EntityInspector
	{
	public:
		EntityInspector() = default;
		EntityInspector(const AssetHandle<Scene>& scene)
			: m_Scene(scene) {}

		void SetScene(const AssetHandle<Scene>& scene);

		void OpenEntity(EntityID entityID);
		void OnImGuiRender();
	private:
		AssetHandle<Scene> m_Scene;
		EntityID m_OpenedEntityID = NullEntityID;

		static void AddComponentPopupMenu(BaseEntity entity);

		static void DrawComponentsTab(Entity entity);

		static void DrawCNameWidget(Entity entity);
		static void DrawDebugTab(Entity entity);
		static void DrawCTransformWidget(Entity entity);
		static void DrawCTagsWidget(Entity entity);

		static void DrawComponentWidget(const ComponentTypesRegistry::DataComponentRegistryItem& item, BaseEntity entity);
	};
}