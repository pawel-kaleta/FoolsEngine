#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class Inspector
	{
	public:
		Inspector() = default;
		Inspector(const Ref<Scene>& scene)
			: m_Scene(scene) {}

		void SetScene(const Ref<Scene>& scene);

		void OpenEntity(EntityID entityID);
		void OnImGuiRender();
	private:
		Ref<Scene> m_Scene;
		EntityID m_OpenedEntityID = NullEntityID;

		static void AddComponentPopupMenu(BaseEntity entity);
		static void AddBehaviorPopupMenu(Actor actor);

		static void DrawCNameWidget(Entity entity);
		static void DrawNodeWidget(Entity entity);
		static void DrawCTransformWidget(Entity entity);
		static void DrawCTagsWidget(Entity entity);

		static void DrawComponentWidget(const ComponentTypesRegistry::DataComponentRegistryItem& item, BaseEntity entity);
		static void DrawBehaviorWidget(const Behavior* behavior);

		void DrawActorInspector();
		void DrawEntityInspector();
	};
}