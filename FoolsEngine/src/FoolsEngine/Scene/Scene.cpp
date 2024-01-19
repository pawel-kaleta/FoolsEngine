#include "FE_pch.h"
#include "Scene.h"

#include "ECS.h"
#include "FoolsEngine\Core\UUID.h"
#include "Component.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "FoolsEngine\Scene\GameplayWorld\Actor\Actor.h"
#include "FoolsEngine\Scene\SimulationStages.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\HierarchyDirector.h"

#include "SceneSerializer.h"

namespace fe
{
	Scene::Scene()
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Scene creation");

		m_GameplayWorld = CreateScope<GameplayWorld>(this);
	}

	

	template <typename tnSimulationStage>
	void Scene::Update()
	{
		FE_PROFILER_FUNC();

		m_GameplayWorld->Update<tnSimulationStage>();
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