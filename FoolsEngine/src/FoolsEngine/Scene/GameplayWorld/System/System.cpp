#include "System.h"

#include "FoolsEngine/Scene/GameplayWorld/GameplayWorld.h"

namespace fe
{
	void System::Initialize()
	{
		OnInitialize();
	}

	void System::Activate()
	{
		if (m_Active)
		{
			Context::Log::Core.Warn("{0} already active"_FE, GetName());
		}

		m_Active = true;
		OnActivate();
	}

	template<SimulationStage::ValueType stage>
	void System::RegisterForUpdate(uint32_t priority)
	{
		FE_PROFILER_FUNC();

		Context::Log::Core.Debug("System Update Register"_FE);

		void (System:: * on_update_func_ptr)() = nullptr;

#define _GET_ON_UPDATE_FUNK_PTR(x) if constexpr (stage == SimulationStage::x) on_update_func_ptr = &System::OnUpdate_##x;
		FE_FOR_EACH(_GET_ON_UPDATE_FUNK_PTR, FE_SIMULATION_STAGES);

		FE_CORE_ASSERT(on_update_func_ptr, "Did not recognise Simulation Stage!");

		m_SystemsDirector->EnrollForUpdate<stage>(this, on_update_func_ptr, priority);
	}

#define _SYSTEM_REGISTER_FOR_UPDATE_DEF(x) template void System::RegisterForUpdate<SimulationStage::x>(uint32_t);
	FE_FOR_EACH(_SYSTEM_REGISTER_FOR_UPDATE_DEF, FE_SIMULATION_STAGES);

	template<SimulationStage::ValueType stage>
	void System::UnregisterFromUpdate()
	{
		FE_PROFILER_FUNC();

		Context::Log::Core.Debug("Behavior Update Unregister"_FE);

		m_SystemsDirector->RemoveUpdateEnroll<stage>(this);
	}

#define _SYSTEM_UREGISTER_FROM_UPDATE_DEF(x) template void System::UnregisterFromUpdate<SimulationStage::x>();
	FE_FOR_EACH(_SYSTEM_UREGISTER_FROM_UPDATE_DEF, FE_SIMULATION_STAGES);

	void System::Deactivate()
	{
		if (!m_Active)
		{
			Context::Log::Core.Warn("{0} already deactivated"_FE, GetName());
		}

		m_Active = false;
		OnDeactivate();

#define _SYSTEM_UREGISTER_FROM_UPDATE_CALL(x) UnregisterFromUpdate<SimulationStage::x>();
		FE_FOR_EACH(_SYSTEM_UREGISTER_FROM_UPDATE_CALL, FE_SIMULATION_STAGES);
	}

	void System::Shutdown()
	{
		OnShutdown();
	}

}