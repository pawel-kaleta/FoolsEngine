#pragma once

#include "FoolsEngine\Scene\World.h"

#include "SimulationStages.h"
#include "Hierarchy\EntitiesHierarchy.h"
#include "ComponentDestructionManager.h"

#include "System.h"

namespace fe
{
	class GameplayWorld : public World
	{
	public:
		GameplayWorld(Scene* scene);

		EntitiesHierarchy& GetHierarchy() { return *m_Hierarchy.get(); }

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
			auto stage = SimulationStages::EnumFromType<tnSimulationStage>();
			UpdateActors(stage, &GameplayWorld::IsActorUpdateEnrolled<tnSimulationStage>);
			UpdateSystems(stage);
		}

		template <typename tnSystem>
		tnSystem* CreateSystem()
		{
			FE_PROFILER_FUNC();
			FE_LOG_CORE_DEBUG("System creation");
			static_assert(std::is_base_of_v<System, tnSystem>, "This is not a behavior!");

			tnSystem* system = new tnSystem();
			system->m_GameplayWorld = this;
			system->OnInitialize();

			{
				std::unique_ptr<System> up(system);
				m_Systems.push_back(std::move(up));
			}
			return system;
		}

	private:
		friend class Entity;
		friend class System;

		Scope<EntitiesHierarchy>	m_Hierarchy;
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
		void UpdateSystems(SimulationStages::Stages stage);

		using Systems = std::vector<std::unique_ptr<System>>;

		struct SystemUpdateEnroll
		{
			System* System;
			void (System::* OnUpdateFuncPtr)();
			uint32_t Priority;
		};
		using SystemUpdateEnrolls = std::array<std::vector<SystemUpdateEnroll>, (int)SimulationStages::Stages::StagesCount>;

		Systems             m_Systems;
		SystemUpdateEnrolls	m_SystemUpdateEnrolls;

		template<typename tnSimulationStage>
		void EnrollForUpdate(System* system, void (System::* onUpdateFuncPtr)(), uint32_t priority)
		{
			FE_LOG_CORE_DEBUG("GameplayWorld: system EnrollForUpdate");

			constexpr int stage = (int)SimulationStages::EnumFromType<tnSimulationStage>();
			m_SystemUpdateEnrolls[stage].push_back(SystemUpdateEnroll{ system, onUpdateFuncPtr, priority });
			SortSystemUpdateEnrolls(stage);
		}

		void SortSystemUpdateEnrolls(int stage);
	};
}