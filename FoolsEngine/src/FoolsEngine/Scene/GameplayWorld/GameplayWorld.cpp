#include "FE_pch.h"
#include "GameplayWorld.h"

#include "Entity.h"
#include "FoolsEngine\Scene\GameplayWorld\Actor\Actor.h"


namespace fe
{
	GameplayWorld::GameplayWorld(Scene* scene)
		: World(scene, true)
	{
		FE_PROFILER_FUNC();

		m_Hierarchy = CreateScope<HierarchyDirector>(this);
		m_SystemsDirector = CreateScope<SystemsDirector>();

		auto root = m_Registry.create(RootID);
		ECS_handle handle(m_Registry, RootID);

		FE_CORE_ASSERT(handle.valid() && handle.entity() == RootID, "Failed to create root Entity in a GameplayWorld");

		m_PersistentToTransientIDsMap[handle.emplace<CUUID>().UUID] = handle.entity();

		handle.emplace<CEntityName>("WorldRoot");
		handle.emplace<CTransformLocal>();
		handle.emplace<CTransformGlobal>();
		handle.emplace<CTags>();
		handle.emplace<CEntityNode>().Parent = NullEntityID;
		handle.emplace<CHeadEntity>().HeadEntity = NullEntityID;
	}

	Entity GameplayWorld::CreateEntity(EntityID parentEntity, const std::string& name)
	{
		FE_PROFILER_FUNC();

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
		FE_PROFILER_FUNC();

		auto& actorStorage = m_Registry.storage<CActorData>();
		auto& nodeStorage = m_Registry.storage<CEntityNode>();
		// should use CActorNode, but CActorNode is not yet mantained
		// because of updateEnrollCheck it is safe to use all entities for traversal

		std::stack<EntityID> toUpdate;
		// some data about entites in underlying hierarchy of just updated actor
		// is going to be already cashed as a result of Actor update (Transform and Tags propagation)
		// so we use stack for tempral locality

		// initializing vector with actors attached to root
		{
			EntityID firstActor = nodeStorage.get(RootID).FirstChild;
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
			if ((this->*updateEnrollCheck)(updateActorID))
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

	Entity GameplayWorld::GetEntityWithPrimaryCamera() const
	{
		Entity entity(m_PrimaryCameraEntityID, this);
		FE_CORE_ASSERT(entity, "Entity with primary camera was deleted");
		FE_CORE_ASSERT(entity.AllOf<CCamera>(), "Primary camera was removed from its Entity");
		return entity;
	}

	void GameplayWorld::SetPrimaryCameraEntity(Entity entity)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Primary Camera Set.");

		if (!(entity))
		{
			FE_CORE_ASSERT(false, "This is not a valid entity");
			return;
		}

		if (!entity.AllOf<CCamera>())
		{
			FE_CORE_ASSERT(false, "This entity does not have a CCamera component");
			return;
		}

		if (entity.m_World != (World*)this)
		{
			FE_CORE_ASSERT(false, "This entity does not belong to GameplayWorld of this Scene");
			return;
		}

		m_PrimaryCameraEntityID = entity.ID();
	}

	void GameplayWorld::SetPrimaryCameraEntity(EntityID id)
	{
		Entity entity(id, this);
		SetPrimaryCameraEntity(entity);
	}
}