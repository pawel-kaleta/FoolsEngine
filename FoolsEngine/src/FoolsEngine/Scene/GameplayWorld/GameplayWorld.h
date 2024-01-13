#pragma once

#include "FoolsEngine\Scene\World.h"

#include "FoolsEngine\Scene\SimulationStages.h"
#include "Hierarchy\HierarchyDirector.h"
#include "ComponentDestructionManager.h"
#include "System\SystemsRegistry.h"
#include "System\SystemsDirector.h"

namespace fe
{
	class GameplayWorld : public World
	{
	public:
		GameplayWorld(Scene* scene);

		HierarchyDirector& GetHierarchy() { return *m_Hierarchy.get(); }
		SystemsDirector& GetSystems()     { return *m_SystemsDirector.get(); }

		Entity CreateEntity(EntityID parentEntity, const std::string& name = "Entity");
		Actor CreateActor(EntityID attachmentEntityID = RootID, const std::string& name = "Actor");
		Actor CreateActor(const std::string& name);

		Actor CreateActorWithUUID(UUID uuid);

		auto AllEntitiesGroup() { return m_Hierarchy->Group(); }

		template <typename tnComponent>
		void ScheduleComponentDestruction(tnComponent* componentPointer, EntityID entityID)
		{
			m_DestructionManager.ScheduleRemoval<tnComponent>(entityID);
		}

		void DestroyScheduledComponents() { m_DestructionManager.DestroyComponents(m_Registry); }

		template <typename tnSimulationStage>
		void Update()
		{
			FE_PROFILER_FUNC();

			auto stage = SimulationStages::EnumFromType<tnSimulationStage>();
			UpdateActors(stage, &GameplayWorld::IsActorUpdateEnrolled<tnSimulationStage>);
			m_SystemsDirector->UpdateSystems(stage);
		}

	private:
		friend class Entity;
		friend class System;

		Scope<HierarchyDirector>	m_Hierarchy;
		Scope<SystemsDirector>      m_SystemsDirector;
		ComponentDestructionManager m_DestructionManager;

		template <typename tnComponent>
		void ScheduleComponentDestructionUnsafe(tnComponent* componentPointer, EntityID entityID)
		{
			m_DestructionManager.ScheduleDestructionUnsafe<tnComponent>(entityID);
		}

		template <typename tnSimulationStage>
		bool IsActorUpdateEnrolled(EntityID actor)
		{
			const static auto& storage = m_Registry.storage<CUpdateEnrollFlag<tnSimulationStage>>();
			return storage.contains(actor);
		}

		void UpdateActors(SimulationStages::Stages stage, bool (GameplayWorld::* updateEnrollCheck)(EntityID));
	};
}