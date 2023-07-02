#pragma once
#include "ECS.h"

namespace fe
{
	using namespace entt::literals;
	class Set;
	class SceneHierarchy;

	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		Set CreateSet(SetID parent = RootID, const std::string& name = "set");
		const Set Root();

		Set GetSetWithPrimaryCamera();
		void SetPrimaryCameraSet(Set set);
		void SetPrimaryCameraSet(SetID id);

		void UpdateScripts();
		void DestroyFlaggedSets();

		Registry& GetRegistry() { return m_Registry; }
		SceneHierarchy& GetHierarchy() { return *m_Hierarchy.get(); }
	private:
		friend class Set;
		friend struct CNativeScript;

		Registry m_Registry;
		SetID m_PrimaryCameraSetID;
		Scope<SceneHierarchy> m_Hierarchy;
	};
}