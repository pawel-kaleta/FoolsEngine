#include "FE_pch.h"
#include "SystemsDirector.h"

#include "SystemsRegistry.h"

namespace fe
{
	void SystemsDirector::UpdateSystems(SimulationStages::Stages stage)
	{
		FE_PROFILER_FUNC();

		for (auto& updateEnroll : m_SystemUpdateEnrolls[(int)stage])
		{
			(updateEnroll.System->*(updateEnroll.OnUpdateFuncPtr))();
		}
	}

	System* SystemsDirector::CreateSystemFromName(const std::string& systemTypeName)
	{
		auto& createPtr = SystemsRegistry::GetInstance().GetItem(systemTypeName)->Create;
		return (this->*createPtr)();
	}

	void SystemsDirector::SortSystemUpdateEnrolls(int stage)
	{
		FE_PROFILER_FUNC();

		auto& enrolls = m_SystemUpdateEnrolls[stage];
		std::sort(
			enrolls.begin(),
			enrolls.end(),
			[](SystemUpdateEnroll& a, SystemUpdateEnroll& b) { return a.Priority < b.Priority; }
		);
	}

	void SystemsDirector::RemoveSystem(System* system)
	{
		bool found = false;
		int position = -1;
		for (int i = 0; i < m_Systems.size(); ++i)
		{
			if (m_Systems[i].get() == system)
			{
				found = true;
				position = i;
				break;
			}
		}

		FE_CORE_ASSERT(found, "System not found");

		auto& enrolls = m_SystemUpdateEnrolls;

		for (int i = 0; i < enrolls.size(); ++i)
		{
			found = false; //reusing foud
			int enrollPos;
			for (int j = 0; j < enrolls[i].size(); ++j)
			{
				if (enrolls[i][j].System == system)
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
			}
		}

		std::swap(m_Systems[position], m_Systems.back());
		system->OnShutdown();
		m_Systems.pop_back();
	}
}