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

		auto& enrolls = m_Data.GetCashed()->m_UpdateEnrolls;
		for (int i = 0; i < enrolls.size(); ++i)
		{
			found = false; //reusing foud
			int enrollPos;
			for (int j = 0; j < enrolls[i].size(); ++j)
			{
				if (enrolls[i][j].Behavior == behavior)
				{
					found = true;
					enrollPos = j;
					break;
				}
			}

			if (found)
			{
				for (int last = enrolls[i].size() - 1; enrollPos < last; ++enrollPos)
				{
					std::swap(enrolls[i][enrollPos], enrolls[i][enrollPos + 1]);
				}

				enrolls[i].pop_back();

				if (enrolls[i].size() == 0)
				{
					static std::array<void (Actor::*)() const, (int)SimulationStages::Stages::StagesCount> unFlagPtrs = 
					{
						&Actor::UnFlag<CUpdateEnrollFlag<SimulationStages::Physics    >>,
						&Actor::UnFlag<CUpdateEnrollFlag<SimulationStages::PostPhysics>>,
						&Actor::UnFlag<CUpdateEnrollFlag<SimulationStages::PrePhysics >>,
						&Actor::UnFlag<CUpdateEnrollFlag<SimulationStages::FrameStart >>,
						&Actor::UnFlag<CUpdateEnrollFlag<SimulationStages::FrameEnd   >>
					};

					(this->*unFlagPtrs[i])();
				}
			}
		}

		std::swap(behaviors[position], behaviors.back());
		behavior->OnShutdown();
		behaviors.pop_back();
	}

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
		FE_PROFILER_FUNC();

		auto& updateEnrolls = m_Data.Get()->m_UpdateEnrolls[stage];
		std::sort(
			updateEnrolls.begin(),
			updateEnrolls.end(),
			[](CActorData::UpdateEnroll& a, CActorData::UpdateEnroll& b) { return a.Priority < b.Priority; }
		);
	}
}