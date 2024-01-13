#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class ActorInspector
	{
	public:
		ActorInspector() = default;
		ActorInspector(const Ref<Scene>& scene)
			: m_Scene(scene) {}

		void SetScene(const Ref<Scene>& scene);

		void OpenActor(EntityID entityID);
		void OnImGuiRender();
	private:
		Ref<Scene> m_Scene;
		EntityID m_OpenedActorID = NullEntityID;
		Behavior* m_BehaviorToRemove = nullptr;

		static void AddBehaviorPopupMenu(Actor actor);
		void DrawBehaviorWidget(Behavior* behavior, Actor actor);

		void DrawCNameWidget(Entity entity);
	};
}