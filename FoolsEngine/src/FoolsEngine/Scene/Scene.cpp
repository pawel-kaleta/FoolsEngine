#include "FE_pch.h"
#include "Scene.h"

#include "ECS.h"
#include "FoolsEngine\Scene\SimulationStages.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\HierarchyDirector.h"

namespace fe
{
	void Scene::Initialize()
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Scene initialization");

		auto& gameplay_world = GetWorlds().GameplayWorld;
		gameplay_world = CreateScope<GameplayWorld>();
		gameplay_world->Initialize();
	}

	template <SimulationStages::Stages stage>
	void Scene::Update()
	{
		FE_PROFILER_FUNC();

		GetWorlds().GameplayWorld->Update<stage>();
	}

	template void Scene::Update<SimulationStages::Stages::FrameStart >();
	template void Scene::Update<SimulationStages::Stages::PrePhysics >();
	template void Scene::Update<SimulationStages::Stages::Physics    >();
	template void Scene::Update<SimulationStages::Stages::PostPhysics>();
	template void Scene::Update<SimulationStages::Stages::FrameEnd   >();

	void Scene::SimulationUpdate()
	{
		FE_PROFILER_FUNC();

		Update<SimulationStages::Stages::FrameStart >();
		Update<SimulationStages::Stages::PrePhysics >();
		Update<SimulationStages::Stages::Physics    >();
		Update<SimulationStages::Stages::PostPhysics>();
		Update<SimulationStages::Stages::FrameEnd   >();
	}

	void Scene::PostFrameUpdate()
	{
		FE_PROFILER_FUNC();

		auto& gameplay_world = GetWorlds().GameplayWorld;

		gameplay_world->DestroyScheduledComponents();
		gameplay_world->GetHierarchy().DestroyFlagged();

		gameplay_world->GetHierarchy().RecreateStorageOrder();
		gameplay_world->GetHierarchy().MakeGlobalTransformsCurrent();
	}

	void Scene::PlaceCoreComponent()
	{
		Emplace<ACWorlds>();
	}

	void Scene::Release()
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Scene release");
		GetWorlds().GameplayWorld.release();
	}
}