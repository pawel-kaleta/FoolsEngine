#pragma once
#include "ECS.h"
#include "Component.h"



namespace fe
{
	class Set;
	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		Set CreateSet();
		const Set Root();

		const Registry& GetRegistry() const { return m_Registry; }
		Set GetSetWithPrimaryCamera();
		void SetPrimaryCameraSet(Set set);
		void SetPrimaryCameraSet(SetID id);

		void UpdateScripts();
	private:
		friend class Set;
		friend struct CNativeScript;
		Registry m_Registry;
		SetID m_PrimaryCameraSetID;
	};

	
}