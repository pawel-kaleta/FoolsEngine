#include "FE_pch.h"
#include "SystemsDirector.h"

#include "SystemsRegistry.h"
#include "FoolsEngine\Core\UUID.h"

namespace fe
{
	void SystemsDirector::UpdateSystems(SimulationStage stage)
	{
		FE_PROFILER_FUNC();

		for (auto& updateEnroll : m_SystemUpdateEnrolls[stage.ToInt()])
		{
			(updateEnroll.System->*(updateEnroll.OnUpdateFuncPtr))();
		}
	}

	template<SimulationStage::ValueType stage>
	void SystemsDirector::EnrollForUpdate(System* system, void(System::* onUpdateFuncPtr)(), uint32_t priority)
	{
		FE_LOG_CORE_DEBUG("GameplayWorld: system EnrollForUpdate");

		m_SystemUpdateEnrolls[stage].push_back(SystemUpdateEnroll{ system, onUpdateFuncPtr, priority });
		SortSystemUpdateEnrolls(stage);
	}

#define _SYSTEMS_DIRECTOR_ENROLL_FOR_UPDATE_DEF(x) template void SystemsDirector::EnrollForUpdate<SimulationStage::x>(System*, void(System::*)(), uint32_t);
	FE_FOR_EACH(_SYSTEMS_DIRECTOR_ENROLL_FOR_UPDATE_DEF, FE_SIMULATION_STAGES);

	template<SimulationStage::ValueType stage>
	void SystemsDirector::RemoveUpdateEnroll(System* system)
	{
		auto& enrolls = m_SystemUpdateEnrolls[stage];

		int found = false;
		int enrollPos;
		for (int j = 0; j < enrolls.size(); ++j)
		{
			if (enrolls[j].System == system)
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
		}
	}

#define _SYSTEMS_DIRECTOR_REMOVE_UPDATE_ENROLL(x) template void SystemsDirector::RemoveUpdateEnroll<SimulationStage::x>(System*);
	FE_FOR_EACH(_SYSTEMS_DIRECTOR_REMOVE_UPDATE_ENROLL, FE_SIMULATION_STAGES);

	System* SystemsDirector::CreateSystemFromName(const std::string& systemTypeName)
	{
		auto* item = SystemsRegistry::GetItem(systemTypeName);
		if (item)
		{
			auto& createPtr = item->Create;
			return (this->*createPtr)();
		}
		return nullptr;
	}

	System* SystemsDirector::GetSystemFromName(const std::string& name)
	{
		for (auto& system : m_Systems)
		{
			if (system->GetName() == name)
			{
				return system.get();
			}
		}
		return nullptr;
	}

	System* SystemsDirector::GetSystemFromUUID(UUID uuid) const
	{
		for (auto& system : m_Systems)
		{
			if (uuid == system->m_UUID)
			{
				return system.get();
			}
		}
		return nullptr;
	}

	void SystemsDirector::SortSystemUpdateEnrolls(SimulationStage stage)
	{
		FE_PROFILER_FUNC();

		auto& enrolls = m_SystemUpdateEnrolls[stage.ToInt()];
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
				for (size_t last = enrolls[i].size() - 1; enrollPos < last; ++enrollPos)
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