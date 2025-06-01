#pragma once

#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "ActorData.h"
#include "FoolsEngine\Scene\GameplayWorld\CompPtr.h"
#include "FoolsEngine\Scene\GameplayWorld\GameplayWorld.h"

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
		
		template <typename tnBehavior>
		tnBehavior* CreateBehavior()
		{
			FE_PROFILER_FUNC();
			FE_LOG_CORE_DEBUG("Behavior creation");
			static_assert(std::is_base_of_v<Behavior, tnBehavior>, "This is not a behavior!");

			tnBehavior* behavior = new tnBehavior();
			m_Data.Get()->m_Behaviors.emplace_back((Behavior*)behavior);

			behavior->m_HeadEntity = Entity(*this);
			behavior->Initialize();
			return behavior;
		}

		void RemoveBehavior(Behavior* behavior);

		void UpdateBehaviors(SimulationStage stage);

		Behavior* GetBehaviorFromUUID(UUID uuid);

		template<SimulationStage::ValueType stage>
		void EnrollForUpdate(Behavior* behavior, void (Behavior::* onUpdateFuncPtr)(), uint32_t priority)
		{
			FE_LOG_CORE_DEBUG("Actor: behavior EnrollForUpdate");

			auto& updateEnrolls = m_Data.Get()->m_UpdateEnrolls[(size_t)stage];
			if (updateEnrolls.size() == 0)
			{
				Flag<CUpdateEnrollFlag<stage>>();
			}
			updateEnrolls.push_back(CActorData::UpdateEnroll{ behavior, onUpdateFuncPtr, priority });
			SortUpdateEnrolls(stage);
		}

		template<SimulationStage::ValueType stage>
		void RemoveUpdateEnroll(Behavior* behavior)
		{
			auto& enrolls =  m_Data.Get()->m_UpdateEnrolls[(size_t)stage];

			int found = false;
			int enrollPos;
			for (int j = 0; j < enrolls.size(); ++j)
			{
				if (enrolls[j].Behavior == behavior)
				{
					found = true;
					enrollPos = j;
					break;
				}
			}

			if (found)
			{
				for (size_t last = enrolls.size() - 1; enrollPos < last; ++enrollPos)
				{
					std::swap(enrolls[enrollPos], enrolls[enrollPos + 1]);
				}

				enrolls.pop_back();

				if (enrolls.size() == 0)
				{
					UnFlag<CUpdateEnrollFlag<stage>>();
				}
			}
		}

	private:
		friend class ActorInspector;
		friend class BehaviorsRegistry;

		CompPtr<CActorData> m_Data;

		void SortUpdateEnrolls(SimulationStage stage);

		template <typename tnBehavior>
		Behavior* CreateBehaviorAsBase()
		{
			return (Behavior*)CreateBehavior<tnBehavior>();
		}
	};
}