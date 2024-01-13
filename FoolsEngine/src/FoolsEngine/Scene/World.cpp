#include "FE_pch.h"
#include "World.h"

#include "BaseEntity.h"

namespace fe
{
	World::World(Scene* scene, bool isGameplayWorld)
		: m_Scene(scene), m_IsGameplayWorld(isGameplayWorld)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("World Creation");
	}

	BaseEntity World::CreateEntity()
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_DEBUG("Entity Creation");

		BaseEntity entity(m_Registry.create(), this);

		m_PersistentToTransientIDsMap[entity.Emplace<CUUID>().UUID] = entity.ID();

		return entity;
	}

	BaseEntity World::CreateEntityWithUUID(UUID uuid)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_DEBUG("Entity Creation with UUID");

		BaseEntity entity(m_Registry.create(), this);

		entity.Emplace<CUUID>().UUID = uuid;
		m_PersistentToTransientIDsMap[uuid] = entity.ID();

		return entity;
	}

	EntityID World::TranslateID(UUID uuid)
	{
		FE_CORE_ASSERT(m_PersistentToTransientIDsMap.find(uuid) != m_PersistentToTransientIDsMap.end(), "UUID not found in Entity UUID to EntityID map");
		return m_PersistentToTransientIDsMap.at(uuid);
	}
}