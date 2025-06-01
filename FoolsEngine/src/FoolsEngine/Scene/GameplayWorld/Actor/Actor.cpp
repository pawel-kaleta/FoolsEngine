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
			RemoveUpdateEnroll<SimulationStage::Physics    >(behavior);
			RemoveUpdateEnroll<SimulationStage::PostPhysics>(behavior);
			RemoveUpdateEnroll<SimulationStage::PrePhysics >(behavior);
			RemoveUpdateEnroll<SimulationStage::FrameStart >(behavior);
			RemoveUpdateEnroll<SimulationStage::FrameEnd   >(behavior);

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
		
		for (auto& updateEnroll : m_Data.GetCashed()->m_UpdateEnrolls[stage.ToInt()])
		{
			auto& beahavior = updateEnroll.Behavior;
			auto& funkPtr   = updateEnroll.OnUpdateFuncPtr;
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

	void Actor::SortUpdateEnrolls(SimulationStage stage)
	{
		FE_PROFILER_FUNC();

		auto& updateEnrolls = m_Data.Get()->m_UpdateEnrolls[stage.ToInt()];
		std::sort(
			updateEnrolls.begin(),
			updateEnrolls.end(),
			[](CActorData::UpdateEnroll& a, CActorData::UpdateEnroll& b) { return a.Priority < b.Priority; }
		);
	}
}