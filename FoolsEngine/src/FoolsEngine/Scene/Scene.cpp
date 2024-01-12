#include "FE_pch.h"
#include "Scene.h"

#include "ECS.h"
#include "FoolsEngine\Core\UUID.h"
#include "Component.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "FoolsEngine\Scene\GameplayWorld\Actor.h"
#include "FoolsEngine\Scene\GameplayWorld\SimulationStages.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\EntitiesHierarchy.h"

namespace fe
{
	Scene::Scene()
		: m_PrimaryCameraEntityID(NullEntityID)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Scene creation");

		m_GameplayWorld = CreateScope<GameplayWorld>(this);
	}

	Entity Scene::GetEntityWithPrimaryCamera() {
		Entity entity(m_PrimaryCameraEntityID, (GameplayWorld*)m_GameplayWorld.get());
		FE_CORE_ASSERT(entity, "Entity with primary camera was deleted");
		FE_CORE_ASSERT(entity.AllOf<CCamera>(), "Primary camera was removed from its Entity");
		return entity;
	}

	void Scene::SetPrimaryCameraEntity(Entity entity)
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

		if (entity.m_World != (GameplayWorld*)m_GameplayWorld.get())
		{
			FE_CORE_ASSERT(false, "This entity does not belong to GameplayWorld of this Scene");
			return;
		}

		m_PrimaryCameraEntityID = entity.ID();
	}

	void Scene::SetPrimaryCameraEntity(EntityID id)
	{
		Entity entity(id, (GameplayWorld*)m_GameplayWorld.get());
		SetPrimaryCameraEntity(entity);
	}

	template <typename tnSimulationStage>
	void Scene::Update()
	{
		FE_PROFILER_FUNC();

		//m_GameplayWorld->UpdateActors(SimulationStages::EnumFromType<tnSimulationStage>());
		m_GameplayWorld->Update<tnSimulationStage>();

		//auto view = m_GameplayWorld->GetRegistry().view<CActorData, CUpdateEnrollFlag<tnSimulationStage>>();

		//for (auto ID : view)
		//{
		//	Actor(ID, m_GameplayWorld.get()).UpdateBehaviors(SimulationStages::EnumFromType<tnSimulationStage>());
		//}
	}

	template void Scene::Update<SimulationStages::FrameStart  >();
	template void Scene::Update<SimulationStages::PrePhysics  >();
	template void Scene::Update<SimulationStages::Physics     >();
	template void Scene::Update<SimulationStages::PostPhysics >();
	template void Scene::Update<SimulationStages::FrameEnd    >();

	void Scene::SimulationUpdate()
	{
		FE_PROFILER_FUNC();

		Update<SimulationStages::FrameStart  >();
		Update<SimulationStages::PrePhysics  >();
		Update<SimulationStages::Physics     >();
		Update<SimulationStages::PostPhysics >();
		Update<SimulationStages::FrameEnd    >();
	}

	void Scene::PostFrameUpdate()
	{
		FE_PROFILER_FUNC();

		m_GameplayWorld->DestroyScheduledComponents();
		m_GameplayWorld->GetHierarchy().DestroyFlagged();

		m_GameplayWorld->GetHierarchy().RecreateStorageOrder();
		m_GameplayWorld->GetHierarchy().MakeGlobalTransformsCurrent();
	}
}