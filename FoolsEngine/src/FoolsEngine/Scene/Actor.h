#pragma once

#include "Entity.h"
#include "ActorData.h"
#include "CompPtr.h"

namespace fe
{
	class Actor : public Entity
	{
	public:
		Actor(Entity entity)
			: Entity(entity), m_Data(entity) {};
		Actor(EntityID entityID, GameplayWorld* world)
			: Entity(entityID, world), m_Data(Entity(entityID, world)) {};
		Actor(const CActorData& actorData, GameplayWorld* world)
			: Entity(world->ComponentToEntity(actorData), world), m_Data(Entity(world->ComponentToEntity(actorData), world)) {};
		
		template <typename tnBahavior>
		tnBahavior* CreateBehavior()
		{
			FE_PROFILER_FUNC();
			FE_LOG_CORE_DEBUG("Behavior creation");
			static_assert(std::is_base_of_v<Behavior, tnBahavior>, "This is not a behavior!");

			tnBahavior* behavior = new tnBahavior();
			behavior->m_HeadEntity = Entity(*this);
			behavior->OnInitialize();

			std::unique_ptr<Behavior> up(behavior);
			m_Data.Get()->m_Behaviors.push_back(std::move(up));
			return behavior;
		}

		void UpdateBehaviors(SimulationStages::Stages stage);

		template<typename tnSimulationStage>
		void EnrollForUpdate(Behavior* localSystem, void (Behavior::* onUpdateFuncPtr)())
		{
			FE_LOG_CORE_DEBUG("Actor: local system EnrollForUpdate");

			constexpr int stage = (int)SimulationStages::EnumFromType<tnSimulationStage>();
			if (m_Data.Get()->m_UpdateEnrolls[stage].size() == 0)
			{
				Flag<CUpdateEnrollFlag<tnSimulationStage>>();
			}
			m_Data.GetCashed()->m_UpdateEnrolls[stage].push_back(CActorData::UpdateEnroll{ localSystem, onUpdateFuncPtr });
		}

	private:
		friend class Inspector;

		CompPtr<CActorData> m_Data;
	};
}