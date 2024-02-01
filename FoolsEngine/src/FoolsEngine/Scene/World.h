#pragma once
#include "ECS.h"

#include "FoolsEngine\Core\UUID.h"

#include <stack>

namespace fe
{
	class Scene;
	class Actor;
	class BaseEntity;

	class World
	{
	public:
		World(Scene* scene, bool isGameplayWorld = false);

		Registry&	GetRegistry()		{ return m_Registry; }
		Scene*		GetScene()			{ return m_Scene; }
		bool		IsGameplayWorld()	{ return m_IsGameplayWorld; }

		EntityID TranslateID(UUID uuid);

		template <typename tnComponent>
		EntityID ComponentToEntity(const tnComponent& component)
		{
			return entt::to_entity(m_Registry.storage<tnComponent>(), component);
		}

		// Use only when deserializing
		BaseEntity CreateOrGetEntityWithUUID(UUID uuid);
	protected:
		friend class Entity;
		friend class Actor;
		friend class EditorLayer;
		friend class WorldHierarchyPanel;

		Registry	m_Registry;
		Scene*		m_Scene				= nullptr;
		bool		m_IsGameplayWorld	= false;

		std::unordered_map<UUID, EntityID>	m_PersistentToTransientIDsMap;

		BaseEntity CreateEntity();
		BaseEntity CreateEntityWithUUID(UUID uuid);
	};

	
}