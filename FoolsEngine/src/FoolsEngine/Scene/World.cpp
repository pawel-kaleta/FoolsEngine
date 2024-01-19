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

	BaseEntity World::CreateOrGetEntityWithUUID(UUID uuid)
	{
		if (0 == uuid)
		{
			return BaseEntity();
		}
		EntityID id = TranslateID(uuid);
		if (id == NullEntityID)
		{
			return CreateEntityWithUUID(uuid);
		}
		return BaseEntity(id, this);
	}

	EntityID World::TranslateID(UUID uuid)
	{
		if (m_PersistentToTransientIDsMap.find(uuid) == m_PersistentToTransientIDsMap.end())
			return NullEntityID;

		return m_PersistentToTransientIDsMap.at(uuid);
	}
}