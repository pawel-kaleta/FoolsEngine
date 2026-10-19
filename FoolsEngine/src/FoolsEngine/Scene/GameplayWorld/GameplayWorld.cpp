#include "GameplayWorld.h"

#include "Entity.h"
#include "Actor/Actor.h"

#include "FoolsEngine/Scene/Components/LightComponents.h"
#include "FoolsEngine/Scene/Components/RenderingComponents.h"

#include <stack>

namespace fe
{
	GameplayWorld::GameplayWorld()
		: World(true), m_PrimaryCameraEntityID(NullEntityID)
	{
		FE_PROFILER_FUNC();

		m_Hierarchy = CreateScope<HierarchyDirector>(this);
		m_SystemsDirector = CreateScope<SystemsDirector>();
	}

	void GameplayWorld::Initialize()
	{
		FE_PROFILER_FUNC();

		EntityID root = m_Registry.create(RootID);
		ECS_handle handle(m_Registry, RootID);

		FE_CORE_ASSERT(handle.valid() && handle.entity() == RootID, "Failed to create root Entity in a GameplayWorld");

		m_PersistentToTransientIDsMap[handle.emplace<CUUID>().UUID] = root;

		handle.emplace<CEntityName>().EntityName.FromConstCharPtr("WorldRoot", 10);
		handle.emplace<CTransformLocal>();
		handle.emplace<CTransformGlobal>();
		handle.emplace<CTags>();
		handle.emplace<CEntityNode>().Parent = NullEntityID;
		handle.emplace<CHeadEntity>().HeadEntity = NullEntityID;
	}

	Entity GameplayWorld::CreateEntity(EntityID parentEntity, String name)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(parentEntity != NullEntityID, "Cannot create a loose entity");
		FE_CORE_ASSERT(parentEntity != RootID, "Cannot attach entity to Root, Create an Actor isnstead");

		Entity entity = World::CreateEntity();

		Context::Log::Core.Error("this is dengerous, we do not know the liftime and original allocator"_FE);
		entity.Emplace<CEntityName>().EntityName = name;

		entity.Emplace<CHeadEntity>().HeadEntity = m_Registry.get<CHeadEntity>(parentEntity).HeadEntity;

		m_Hierarchy->CreateNode(entity, parentEntity);

		return entity;
	}

	Actor GameplayWorld::CreateActor(EntityID attachmentEntityID, String name)
	{
		FE_PROFILER_FUNC();
		Context::Log::Core.Debug("Actor Creation"_FE);

		FE_CORE_ASSERT(attachmentEntityID != NullEntityID, "Cannot create a loose actor");

		Entity entity = World::CreateEntity();
		Context::Log::Core.Error("this is dengerous, we do not know the liftime and original allocator"_FE);
		entity.Emplace<CEntityName>().EntityName = name;
		m_Hierarchy->CreateNode(entity, attachmentEntityID);
		entity.Emplace<CHeadEntity>().HeadEntity = entity.ID();
		m_Registry.emplace<CActorData>(entity.ID(), entity.ID());
		return Actor(entity);
	}

	Actor GameplayWorld::CreateActor(String name)
	{
		return CreateActor(RootID, name);
	}

	Actor GameplayWorld::CreateActor(EntityID attachmentEntityID)
	{
		return CreateActor(attachmentEntityID, String("Actor"_FE));
	}

	Actor GameplayWorld::CreateActor()
	{
		return CreateActor(RootID, String("Actor"_FE));
	}

	Actor GameplayWorld::CreateActorWithUUID(UUID uuid)
	{
		FE_PROFILER_FUNC();

		Entity ent = CreateOrGetEntityWithUUID(uuid);

		auto& actor_data = m_Registry.emplace<CActorData>(ent.ID(), ent.ID());
		return Actor(actor_data, this);
	}

	template <SimulationStage::ValueType stage>
	void GameplayWorld::Update()
	{
		FE_PROFILER_FUNC();

		UpdateActors(stage, &GameplayWorld::IsActorUpdateEnrolled<stage>);
		m_SystemsDirector->UpdateSystems(stage);
	}

#define _GAMEPLAY_WORLD_UPDATE_DEF(x) template void GameplayWorld::Update<SimulationStage::x>();
	FE_FOR_EACH(_GAMEPLAY_WORLD_UPDATE_DEF, FE_SIMULATION_STAGES);

	void GameplayWorld::UpdateActors(SimulationStage stage, bool (GameplayWorld::* updateEnrollCheck)(EntityID))
	{
		FE_PROFILER_FUNC();

		Pile p;

		auto& node_storage = m_Registry.storage<CEntityNode>();
		// should use CActorNode, but CActorNode is not yet mantained
		// because of updateEnrollCheck it is safe to use all entities for traversal

		DynamicArenaSA<EntityID, Pile> actors_to_update;
		actors_to_update.Init(&p);
		// some data about entites in underlying hierarchy of just updated actor
		// is going to be already cashed as a result of Actor update (Transform and Tags propagation)
		// so we use stack for tempral locality

		// initializing vector with actors attached to root
		{
			EntityID first_actor = node_storage.get(RootID).FirstChild;
			EntityID current_actor = first_actor;

			if (current_actor != NullEntityID)
				do
				{
					actors_to_update.Append(current_actor);
					current_actor = node_storage.get(current_actor).NextSibling;
				} while (current_actor != first_actor && current_actor != NullEntityID);
		}

		EntityID current_actorID;

		// this can be parallelised
		while (actors_to_update.Count)
		{
			current_actorID = actors_to_update.PopBack();

			// UpdateBehaviors() would access ActorData polluting cashe, even if there are no behaviors to run
			// CUpdateEnrollFlag<> has empty component optimisation
			if ((this->*updateEnrollCheck)(current_actorID))
				Actor(current_actorID, this).UpdateBehaviors(stage);

			// scheduling of children
			EntityID first_sibling = node_storage.get(current_actorID).FirstChild;
			EntityID current = first_sibling;

			if (current == NullEntityID)
				continue;

			do
			{
				actors_to_update.Append(current);
				current = node_storage.get(current).NextSibling;
			} while (current != first_sibling && current != NullEntityID);
		}
	}

	Entity GameplayWorld::GetEntityWithPrimaryCamera() const
	{
		Entity entity(m_PrimaryCameraEntityID, this);
		// are those ifs neccessary?
		if (entity)
			if (!entity.AllOf<CCamera>())
				return Entity();
		return entity;
	}

	void GameplayWorld::SetPrimaryCameraEntity(Entity entity)
	{
		FE_PROFILER_FUNC();
		Context::Log::Core.Info("Primary Camera Set."_FE);

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

	Entity GameplayWorld::GetEntityWithPrimaryDirectionalLight() const
	{
		Entity entity(m_PrimaryDirectionalLightEntityID, this);
		// are those ifs neccessary?
		if (entity)
			if (!entity.AllOf<CDirectionalLight>())
				return Entity();
		return entity;
	}

	void GameplayWorld::SetPrimaryDirectionalLightEntity(Entity entity)
	{
		FE_PROFILER_FUNC();
		Context::Log::Core.Info("Primary Directional Light Set."_FE);

		if (!(entity))
		{
			FE_CORE_ASSERT(false, "This is not a valid entity");
			return;
		}

		if (!entity.AllOf<CDirectionalLight>())
		{
			FE_CORE_ASSERT(false, "This entity does not have a CDirectionalLight component");
			return;
		}

		if (entity.m_World != (World*)this)
		{
			FE_CORE_ASSERT(false, "This entity does not belong to GameplayWorld of this Scene");
			return;
		}

		m_PrimaryDirectionalLightEntityID = entity.ID();
	}

	void GameplayWorld::SetPrimaryDirectionalLightEntity(EntityID id)
	{
		Entity entity(id, this);
		SetPrimaryDirectionalLightEntity(entity);
	}
}