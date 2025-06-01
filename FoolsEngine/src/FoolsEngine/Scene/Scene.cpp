#include "FE_pch.h"
#include "Scene.h"

#include "ECS.h"
#include "FoolsEngine\Scene\SimulationStage.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\HierarchyDirector.h"

namespace fe
{
	void SceneUser::Initialize() const
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Scene initialization");

		auto& gameplay_world = GetCoreComponent().GameplayWorld;
		gameplay_world = CreateScope<GameplayWorld>();
		gameplay_world->Initialize();
	}

	template <SimulationStage::ValueType stage>
	void SceneUser::Update() const
	{
		FE_PROFILER_FUNC();

		GetCoreComponent().GameplayWorld->Update<stage>();
	}

	template void SceneUser::Update<SimulationStage::FrameStart >() const;
	template void SceneUser::Update<SimulationStage::PrePhysics >() const;
	template void SceneUser::Update<SimulationStage::Physics    >() const;
	template void SceneUser::Update<SimulationStage::PostPhysics>() const;
	template void SceneUser::Update<SimulationStage::FrameEnd   >() const;

	void SceneUser::SimulationUpdate() const
	{
		FE_PROFILER_FUNC();

		Update<SimulationStage::FrameStart >();
		Update<SimulationStage::PrePhysics >();
		Update<SimulationStage::Physics    >();
		Update<SimulationStage::PostPhysics>();
		Update<SimulationStage::FrameEnd   >();
	}

	void SceneUser::PostFrameUpdate() const
	{
		FE_PROFILER_FUNC();

		auto& gameplay_world = GetCoreComponent().GameplayWorld;

		gameplay_world->DestroyScheduledComponents();
		gameplay_world->GetHierarchy().DestroyFlagged();

		gameplay_world->GetHierarchy().RecreateStorageOrder();
		gameplay_world->GetHierarchy().MakeGlobalTransformsCurrent();
	}

	void SceneUser::Release() const
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Scene release");
		GetCoreComponent().GameplayWorld.release();
	}
}