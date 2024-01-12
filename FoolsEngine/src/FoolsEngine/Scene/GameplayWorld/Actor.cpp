#include "FE_pch.h"
#include "Actor.h"

namespace fe
{
	void Actor::UpdateBehaviors(SimulationStages::Stages stage)
	{
		FE_PROFILER_FUNC();

		m_Data.GetAndCash();
		
		for (auto& updateEnroll : m_Data.GetCashed()->m_UpdateEnrolls[(int)stage])
		{
			(updateEnroll.Behavior->*(updateEnroll.OnUpdateFuncPtr))();
		}
	}

	void Actor::SortUpdateEnrolls(int stage)
	{
		auto& updateEnrolls = m_Data.Get()->m_UpdateEnrolls[stage];
		std::sort(
			updateEnrolls.begin(),
			updateEnrolls.end(),
			[](CActorData::UpdateEnroll& a, CActorData::UpdateEnroll& b) { return a.Priority < b.Priority; }
		);
	}
}