#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class ActorInspector
	{
	public:
		ActorInspector() = default;
		ActorInspector(const AssetHandle<Scene>& scene)
			: m_Scene(scene) {}

		void SetScene(const AssetHandle<Scene>& scene);

		void     OpenActor(EntityID entityID);
		EntityID GetSelectionRequest() const { return m_EntityIDSelectionRequest; }
		void     OnImGuiRender();
	private:
		AssetHandle<Scene> m_Scene;
		EntityID   m_OpenedActorID    = NullEntityID;
		Behavior*  m_BehaviorToRemove = nullptr;
		EntityID   m_SelectedEntityID = NullEntityID;
		EntityID   m_EntityIDSelectionRequest = NullEntityID;

		static void AddBehaviorPopupMenu(Actor actor);
		void DrawBehaviorWidget(Behavior* behavior, Actor actor);

		void DrawCNameWidget(Entity entity);
	};
}