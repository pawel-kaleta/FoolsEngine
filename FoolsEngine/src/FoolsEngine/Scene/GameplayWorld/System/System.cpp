#include "FE_pch.h"
#include "System.h"

#include "FoolsEngine\Scene\GameplayWorld\GameplayWorld.h"

namespace fe
{
	void System::Initialize()
	{
		OnInitialize();
	}

	void System::Activate()
	{
		if (m_Active)
			FE_LOG_CORE_WARN("{0} already active", GetName());

		m_Active = true;
		OnActivate();
	}

	template<SimulationStage::ValueType stage>
	void System::RegisterForUpdate(uint32_t priority)
	{
		FE_PROFILER_FUNC();

		FE_LOG_CORE_DEBUG("System Update Register");

		void (System:: * onUpdateFuncPtr)() = nullptr;

#define _GET_ON_UPDATE_FUNK_PTR(x) if constexpr (stage == SimulationStage::x) onUpdateFuncPtr = &System::OnUpdate_##x;
		FE_FOR_EACH(_GET_ON_UPDATE_FUNK_PTR, FE_SIMULATION_STAGES);

		FE_CORE_ASSERT(onUpdateFuncPtr, "Did not recognise Simulation Stage!");

		m_SystemsDirector->EnrollForUpdate<stage>(this, onUpdateFuncPtr, priority);
	}

#define _SYSTEM_REGISTER_FOR_UPDATE_DEF(x) template void System::RegisterForUpdate<SimulationStage::x>(uint32_t);
	FE_FOR_EACH(_SYSTEM_REGISTER_FOR_UPDATE_DEF, FE_SIMULATION_STAGES);

	template<SimulationStage::ValueType stage>
	void System::UnregisterFromUpdate()
	{
		FE_PROFILER_FUNC();

		FE_LOG_CORE_DEBUG("Behavior Update Unregister");

		m_SystemsDirector->RemoveUpdateEnroll<stage>(this);
	}

#define _SYSTEM_UREGISTER_FROM_UPDATE_DEF(x) template void System::UnregisterFromUpdate<SimulationStage::x>();
	FE_FOR_EACH(_SYSTEM_UREGISTER_FROM_UPDATE_DEF, FE_SIMULATION_STAGES);

	void System::Deactivate()
	{
		if (!m_Active)
			FE_LOG_CORE_WARN("{0} already deactivated", GetName());

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