#include "FE_pch.h"
#include "Scene.h"

#include "ECS.h"
#include "FoolsEngine\Scene\SimulationStages.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\HierarchyDirector.h"

namespace fe
{
	void SceneUser::Initialize() const
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Scene initialization");

		auto& gameplay_world = GetWorlds().GameplayWorld;
		gameplay_world = CreateScope<GameplayWorld>();
		gameplay_world->Initialize();
	}

	template <SimulationStages::Stages stage>
	void SceneUser::Update() const
	{
		FE_PROFILER_FUNC();

		GetWorlds().GameplayWorld->Update<stage>();
	}

	template void SceneUser::Update<SimulationStages::Stages::FrameStart >() const;
	template void SceneUser::Update<SimulationStages::Stages::PrePhysics >() const;
	template void SceneUser::Update<SimulationStages::Stages::Physics    >() const;
	template void SceneUser::Update<SimulationStages::Stages::PostPhysics>() const;
	template void SceneUser::Update<SimulationStages::Stages::FrameEnd   >() const;

	void SceneUser::SimulationUpdate() const
	{
		FE_PROFILER_FUNC();

		Update<SimulationStages::Stages::FrameStart >();
		Update<SimulationStages::Stages::PrePhysics >();
		Update<SimulationStages::Stages::Physics    >();
		Update<SimulationStages::Stages::PostPhysics>();
		Update<SimulationStages::Stages::FrameEnd   >();
	}

	void SceneUser::PostFrameUpdate() const
	{
		FE_PROFILER_FUNC();

		auto& gameplay_world = GetWorlds().GameplayWorld;

		gameplay_world->DestroyScheduledComponents();
		gameplay_world->GetHierarchy().DestroyFlagged();

		gameplay_world->GetHierarchy().RecreateStorageOrder();
		gameplay_world->GetHierarchy().MakeGlobalTransformsCurrent();
	}

	void SceneUser::PlaceCoreComponent() const
	{
		Emplace<ACWorlds>();
	}

	void SceneUser::Release() const
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Scene release");
		GetWorlds().GameplayWorld.release();
	}
}