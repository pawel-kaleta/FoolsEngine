#include "FE_pch.h"
#include "Scene.h"

#include "ECS.h"
#include "FoolsEngine\Core\UUID.h"
#include "Component.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "FoolsEngine\Scene\GameplayWorld\Actor\Actor.h"
#include "FoolsEngine\Scene\SimulationStages.h"
#include "FoolsEngine\Scene\GameplayWorld\Hierarchy\HierarchyDirector.h"

#include "FoolsEngine\Assets\Serializers\SceneSerializer.h"

namespace fe
{
	Scene::Scene()
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_INFO("Scene creation");

		m_GameplayWorld = CreateScope<GameplayWorld>(this);
	}

	void Scene::Initialize()
	{
		m_GameplayWorld->Initialize();
	}

	template <SimulationStages::Stages stage>
	void Scene::Update()
	{
		FE_PROFILER_FUNC();

		m_GameplayWorld->Update<stage>();
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

		m_GameplayWorld->DestroyScheduledComponents();
		m_GameplayWorld->GetHierarchy().DestroyFlagged();

		m_GameplayWorld->GetHierarchy().RecreateStorageOrder();
		m_GameplayWorld->GetHierarchy().MakeGlobalTransformsCurrent();
	}
}