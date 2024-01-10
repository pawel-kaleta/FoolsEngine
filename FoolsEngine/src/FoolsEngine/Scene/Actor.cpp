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
}