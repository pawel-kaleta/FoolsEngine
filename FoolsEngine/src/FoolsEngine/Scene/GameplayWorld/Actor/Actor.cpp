#include "Actor.h"

namespace fe
{
	void Actor::RemoveBehavior(Behavior* behavior)
	{
		bool found = false;
		int position = -1;
		auto& behaviors = m_Data.Get()->m_Behaviors;
		for (int i = 0; i < behaviors.Count; ++i)
		{
			if (behaviors[i] == behavior)
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

			std::swap(behaviors[position], behaviors[behaviors.Count - 1]);
			if (behavior->m_Active)
				behavior->Deactivate();
			behavior->Shutdown();
			behaviors.PopBack();
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
				return behavior;
			}
		}
		return nullptr;
	}

	template<SimulationStage::ValueType stage>
	void Actor::EnrollForUpdate(Behavior* behavior, void (Behavior::* onUpdateFuncPtr)(), uint32_t priority)
	{
		auto& update_enrolls = m_Data.Get()->m_UpdateEnrolls[stage];
		if (update_enrolls.Count == 0)
		{
			Flag<CUpdateEnrollFlag<stage>>();
		}
		update_enrolls.Append(CActorData::UpdateEnroll{ behavior, onUpdateFuncPtr, priority });
		SortUpdateEnrolls(stage);
	}

#define _ACTOR_ENROLL_FOR_UPDATE_DEF(x) template void Actor::EnrollForUpdate<SimulationStage::x>(Behavior* behavior, void (Behavior::* onUpdateFuncPtr)(), uint32_t priority);
	FE_FOR_EACH(_ACTOR_ENROLL_FOR_UPDATE_DEF, FE_SIMULATION_STAGES);

	template<SimulationStage::ValueType stage>
	void Actor::RemoveUpdateEnroll(Behavior* behavior)
	{
		auto& enrolls = m_Data.Get()->m_UpdateEnrolls[stage];

		for (auto& enroll : enrolls)
		{
			if (enroll.Behavior != behavior)
				continue;

			UInt enroll_position = enroll.P() - enrolls.begin();
			UInt move_size = ((Byte*)enrolls.end() - (Byte*)enroll.P()) - sizeof(enroll);

			if (move_size)
			{
				Pile p;
				auto tmp = p.Allocate<CActorData::UpdateEnroll>(move_size);
				std::memcpy(tmp.Elements, enrolls[enroll_position].P(), move_size);
				std::memcpy(enrolls[enroll_position - 1].P(), tmp.Elements, move_size);
			}
			enrolls.PopBack();

			if (enrolls.Count == 0)
			{
				UnFlag<CUpdateEnrollFlag<stage>>();
			}

			return;
		}

		FE_ASSERT(false, "Behavior cannot be unenrolled for update as it is not enrolled");
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