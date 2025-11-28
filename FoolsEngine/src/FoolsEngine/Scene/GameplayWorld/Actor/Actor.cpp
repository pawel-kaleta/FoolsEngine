#include "FE_pch.h"
#include "Actor.h"

namespace fe
{
	void Actor::RemoveBehavior(Behavior* behavior)
	{
		bool found = false;
		int position = -1;
		auto& behaviors = m_Data.Get()->m_Behaviors;
		for (int i = 0; i < behaviors.size(); ++i)
		{
			if (behaviors[i].get() == behavior)
			{
				found = true;
				position = i;
				break;
			}
		}

		FE_CORE_ASSERT(found, "This behavior does not belong to this actor");

		if (found)
		{
#define _ACTOR_REMOVE_UPDATE_ENROLL_CALL(x) RemoveUpdateEnroll<SimulationStage::x>(behavior);
			FE_FOR_EACH(_ACTOR_REMOVE_UPDATE_ENROLL_CALL, FE_SIMULATION_STAGES);

			std::swap(behaviors[position], behaviors.back());
			if (behavior->m_Active)
				behavior->Deactivate();
			behavior->Shutdown();
			behaviors.pop_back();
		}

		return;
	}

	void Actor::UpdateBehaviors(SimulationStage stage)
	{
		FE_PROFILER_FUNC();

		m_Data.GetAndCash();
		
		for (auto& update_enroll : m_Data.GetCashed()->m_UpdateEnrolls[stage.ToInt()])
		{
			auto& beahavior = update_enroll.Behavior;
			auto& funkPtr   = update_enroll.OnUpdateFuncPtr;
			(beahavior->*funkPtr)();
		}
	}

	Behavior* Actor::GetBehaviorFromUUID(UUID uuid)
	{
		for (auto& behavior : m_Data.Get()->m_Behaviors)
		{
			if (behavior->m_UUID == uuid)
			{
				return behavior.get();
			}
		}
		return nullptr;
	}

	template<SimulationStage::ValueType stage>
	void Actor::EnrollForUpdate(Behavior* behavior, void (Behavior::* onUpdateFuncPtr)(), uint32_t priority)
	{
		auto& update_enrolls = m_Data.Get()->m_UpdateEnrolls[stage];
		if (update_enrolls.size() == 0)
		{
			Flag<CUpdateEnrollFlag<stage>>();
		}
		update_enrolls.push_back(CActorData::UpdateEnroll{ behavior, onUpdateFuncPtr, priority });
		SortUpdateEnrolls(stage);
	}

#define _ACTOR_ENROLL_FOR_UPDATE_DEF(x) template void Actor::EnrollForUpdate<SimulationStage::x>(Behavior* behavior, void (Behavior::* onUpdateFuncPtr)(), uint32_t priority);
	FE_FOR_EACH(_ACTOR_ENROLL_FOR_UPDATE_DEF, FE_SIMULATION_STAGES);

	template<SimulationStage::ValueType stage>
	void Actor::RemoveUpdateEnroll(Behavior* behavior)
	{
		auto& enrolls = m_Data.Get()->m_UpdateEnrolls[stage];

		int found = false;
		int enroll_position;
		for (int j = 0; j < enrolls.size(); ++j)
		{
			if (enrolls[j].Behavior == behavior)
			{
				found = true;
				enroll_position = j;
				break;
			}
		}

		if (found)
		{
			for (size_t last = enrolls.size() - 1; enroll_position < last; ++enroll_position)
			{
				std::swap(enrolls[enroll_position], enrolls[enroll_position + 1]);
			}

			enrolls.pop_back();

			if (enrolls.size() == 0)
			{
				UnFlag<CUpdateEnrollFlag<stage>>();
			}
		}
	}

#define _ACTOR_REMOVE_UPDATE_ENROLL_DEF(x) template void Actor::RemoveUpdateEnroll<SimulationStage::x>(Behavior* behavior);
	FE_FOR_EACH(_ACTOR_REMOVE_UPDATE_ENROLL_DEF, FE_SIMULATION_STAGES);

	void Actor::SortUpdateEnrolls(SimulationStage stage)
	{
		FE_PROFILER_FUNC();

		auto& update_enrolls = m_Data.Get()->m_UpdateEnrolls[stage.ToInt()];
		std::sort(
			update_enrolls.begin(),
			update_enrolls.end(),
			[](CActorData::UpdateEnroll& a, CActorData::UpdateEnroll& b) { return a.Priority < b.Priority; }
		);
	}
}