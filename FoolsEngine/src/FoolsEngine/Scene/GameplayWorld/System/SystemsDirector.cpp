#include "SystemsDirector.h"

#include "SystemTypesRegistry.h"

#include "FoolsEngine/Application/UUID.h"

namespace fe
{
	void SystemsDirector::UpdateSystems(SimulationStage stage)
	{
		FE_PROFILER_FUNC();

		for (auto& update_enroll : m_SystemUpdateEnrolls[stage.ToInt()])
		{
			(update_enroll.System->*(update_enroll.OnUpdateFuncPtr))();
		}
	}

	template<SimulationStage::ValueType stage>
	void SystemsDirector::EnrollForUpdate(System* system, void(System::* onUpdateFuncPtr)(), uint32_t priority)
	{
		FE_LOG_CORE_DEBUG("GameplayWorld: system EnrollForUpdate");
		
		auto& enrolls = m_SystemUpdateEnrolls[stage];
		
		UInt position = 0;
		for (auto& enroll : enrolls)
		{
			if (enroll.Priority > priority)
			{
				position += 1;
				continue;
			}

			auto new_enroll = enrolls.Emplace(position);

			new_enroll->System = system;
			new_enroll->OnUpdateFuncPtr = onUpdateFuncPtr;
			new_enroll->Priority = priority;

			return;
		}

		auto new_enroll = enrolls.EmplaceBack();

		new_enroll->System = system;
		new_enroll->OnUpdateFuncPtr = onUpdateFuncPtr;
		new_enroll->Priority = priority;
	}

#define _SYSTEMS_DIRECTOR_ENROLL_FOR_UPDATE_DEF(x) template void SystemsDirector::EnrollForUpdate<SimulationStage::x>(System*, void(System::*)(), uint32_t);
	FE_FOR_EACH(_SYSTEMS_DIRECTOR_ENROLL_FOR_UPDATE_DEF, FE_SIMULATION_STAGES);

	template<SimulationStage::ValueType stage>
	void SystemsDirector::RemoveUpdateEnroll(System* system)
	{
		auto& enrolls = m_SystemUpdateEnrolls[stage];

		SystemUpdateEnroll* enroll_to_remove = nullptr;
		for (auto& enroll : enrolls)
		{
			if (enroll.System != system)
				continue;

			enroll_to_remove = &enroll;
		}

		if (!enroll_to_remove)
		{
			FE_CORE_ASSERT(false, "System update enroll for update not found for removal");
			return;
		}

		SystemUpdateEnroll* first_to_move = enroll_to_remove + 1;
		UInt move_size = (enrolls[enrolls.Count].P() - first_to_move) * sizeof(SystemUpdateEnroll);

		Pile p;
		auto tmp = p.Allocate< SystemUpdateEnroll>(move_size);
		std::memcpy(tmp.Elements, first_to_move, move_size);
		std::memcpy(enroll_to_remove, tmp.Elements, move_size);

		enrolls.PopBack();
	}

#define _SYSTEMS_DIRECTOR_REMOVE_UPDATE_ENROLL(x) template void SystemsDirector::RemoveUpdateEnroll<SimulationStage::x>(System*);
	FE_FOR_EACH(_SYSTEMS_DIRECTOR_REMOVE_UPDATE_ENROLL, FE_SIMULATION_STAGES);

	System* SystemsDirector::CreateSystemFromName(String systemTypeName)
	{
		auto* item = SystemTypesRegistry::GetItem(systemTypeName);
		if (item)
		{
			auto& create_funk_ptr = item->Create;
			return (this->*create_funk_ptr)();
		}
		return nullptr;
	}

	System* SystemsDirector::GetSystemFromName(String name) const
	{
		for (auto& system : m_Systems)
		{
			if (CompareStringsEqual(system->GetName(), name))
			{
				return system;
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
				return system;
			}
		}
		return nullptr;
	}

	void SystemsDirector::RemoveSystem(System* system)
	{
		bool found = false;
		for (auto& current_system : m_Systems)
		{
			if (current_system != system)
				continue;
			
			UInt position = (& current_system - & m_Systems[0]);
			m_Systems.SwapWithBackAndPop(position);
			found = true;
		}

		if (!found)
		{
			FE_CORE_ASSERT(false, "System not found");
			return;
		}

		auto& enrolls = m_SystemUpdateEnrolls;

		for (auto& stage_enrolls : enrolls)
		{
			for (auto& enroll : stage_enrolls)
			{
				if (enroll.System != system)
					continue;

				found = true;
				UInt enrollPos = (enroll.P() - stage_enrolls[0].P());

				SystemUpdateEnroll* first_to_move = enroll.P() + 1;
				UInt move_size = (stage_enrolls[stage_enrolls.Count].P() - first_to_move) * sizeof(SystemUpdateEnroll);

				Pile p;
				auto tmp = p.Allocate<SystemUpdateEnroll>(move_size);
				std::memcpy(tmp.Elements, first_to_move, move_size);
				std::memcpy(enroll.P(), tmp.Elements, move_size);

				stage_enrolls.PopBack();

				break;
			}
		}

		system->OnShutdown();
		FE_CORE_ASSERT(false, "Not implemented"); // deallocate system ?
	}
}