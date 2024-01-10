#include "FE_pch.h"
#include "World.h"
#include "Actor.h"
#include "Entity.h"

namespace fe
{
	World::World(Scene* scene, bool isGameplayWorld)
		: m_Scene(scene), m_IsGameplayWorld(isGameplayWorld)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("GameplayWorld Creation");

		auto root = m_Registry.create(RootID);
		ECS_handle handle(m_Registry, RootID);

		FE_CORE_ASSERT(handle.valid() && handle.entity() == RootID, "Failed to create root Entity in a GameplayWorld");

		m_PersistentToTransientIDsMap[handle.emplace<CUUID>().UUID] = handle.entity();
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

	GameplayWorld::GameplayWorld(Scene* scene)
		: World(scene, true)
	{
		m_Hierarchy = CreateScope<EntitiesHierarchy>(this);

		ECS_handle handle(m_Registry, RootID);

		handle.emplace<CEntityName>("WorldRoot");
		handle.emplace<CTransformLocal>();
		handle.emplace<CTransformGlobal>();
		handle.emplace<CTags>();
		handle.emplace<CEntityNode>().Parent = NullEntityID;
		handle.emplace<CHeadEntity>().HeadEntity = NullEntityID;
	}

	Entity GameplayWorld::CreateEntity(EntityID parentEntity, const std::string& name)
	{
		FE_CORE_ASSERT(parentEntity != NullEntityID, "Cannot create a loose entity");
		FE_CORE_ASSERT(parentEntity != RootID, "Cannot attach entity to Root, Create an Actor isnstead");

		Entity entity = World::CreateEntity();

		entity.Emplace<CEntityName>(name);
		
		entity.Emplace<CHeadEntity>().HeadEntity = m_Registry.get<CHeadEntity>(parentEntity).HeadEntity;
		
		m_Hierarchy->CreateNode(entity, parentEntity);

		return entity;
	}

	Actor GameplayWorld::CreateActor(EntityID attachmentEntityID, const std::string& name)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_DEBUG("Actor Creation");

		FE_CORE_ASSERT(attachmentEntityID != NullEntityID, "Cannot create a loose actor");

		Entity entity = World::CreateEntity();
		entity.Emplace<CEntityName>(name);
		m_Hierarchy->CreateNode(entity, attachmentEntityID);
		entity.Emplace<CHeadEntity>().HeadEntity = entity.ID();
		auto& actorData = m_Registry.emplace<CActorData>(entity.ID());
		return Actor(entity);
	}

	Actor GameplayWorld::CreateActor(const std::string& name)
	{
		return CreateActor(RootID, name);
	}

	Actor GameplayWorld::CreateActorWithUUID(UUID uuid)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_DEBUG("Actor Creation with UUID");

		Entity ent = GameplayWorld::CreateEntityWithUUID(uuid);
		
		ent.Emplace<CHeadEntity>().HeadEntity = ent.ID();
		auto& actorData = m_Registry.emplace<CActorData>(ent.ID());
		return Actor(actorData, this);
	}

	void GameplayWorld::UpdateActors(SimulationStages::Stages stage, bool (GameplayWorld::* updateEnrollCheck)(EntityID))
	{
		auto& actorStorage = m_Registry.storage<CActorData>();
		auto& nodeStorage = m_Registry.storage<CEntityNode>();

		std::stack<EntityID> toUpdate;
		// some data about entites in underlying hierarchy of just updated actor
		// is going to be already cashed as a result of Actor update (Transform and Tags propagation)
		// so we use stack for tempral locality

		// initializing vector with actors attached to root
		{
			EntityID firstActor = m_Registry.get<CEntityNode>(RootID).FirstChild;
			EntityID currentActor = firstActor;

			if (currentActor != NullEntityID)
				do
				{
					toUpdate.push(currentActor);
					currentActor = nodeStorage.get(currentActor).NextSibling;
				} while (currentActor != firstActor && currentActor != NullEntityID);
		}

		EntityID updateActorID;

		// this can be parallelised
		while (toUpdate.size())
		{
			updateActorID = toUpdate.top();
			toUpdate.pop();

			// UpdateBehaviors() would access ActorData polluting cashe, even if there are no behaviors to run
			// CUpdateEnrollFlag<> has empty component optimisation
			if ( (this->*updateEnrollCheck)(updateActorID))
				Actor(updateActorID, this).UpdateBehaviors(stage);

			// scheduling of children
			{
				EntityID firstSibling = nodeStorage.get(updateActorID).FirstChild;
				EntityID current = firstSibling;

				if (current != NullEntityID)
					do
					{
						toUpdate.push(current);
						current = nodeStorage.get(current).NextSibling;
					} while (current != firstSibling && current != NullEntityID);
			}
		}
	}
}