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

		if constexpr (stage == SimulationStage::FrameStart ) onUpdateFuncPtr = &System::OnUpdate_FrameStart;
		if constexpr (stage == SimulationStage::PrePhysics ) onUpdateFuncPtr = &System::OnUpdate_PrePhysics;
		if constexpr (stage == SimulationStage::Physics    ) onUpdateFuncPtr = &System::OnUpdate_Physics;
		if constexpr (stage == SimulationStage::PostPhysics) onUpdateFuncPtr = &System::OnUpdate_PostPhysics;
		if constexpr (stage == SimulationStage::FrameEnd   ) onUpdateFuncPtr = &System::OnUpdate_FrameEnd;

		FE_CORE_ASSERT(onUpdateFuncPtr, "Did not recognise Simulation Stage!");

		m_SystemsDirector->EnrollForUpdate<stage>(this, onUpdateFuncPtr, priority);
	}

	template void System::RegisterForUpdate<SimulationStage::FrameStart >(uint32_t);
	template void System::RegisterForUpdate<SimulationStage::PrePhysics >(uint32_t);
	template void System::RegisterForUpdate<SimulationStage::Physics    >(uint32_t);
	template void System::RegisterForUpdate<SimulationStage::PostPhysics>(uint32_t);
	template void System::RegisterForUpdate<SimulationStage::FrameEnd   >(uint32_t);

	template<SimulationStage::ValueType stage>
	void System::UnregisterFromUpdate()
	{
		FE_PROFILER_FUNC();

		FE_LOG_CORE_DEBUG("Behavior Update Unregister");

		m_SystemsDirector->RemoveUpdateEnroll<stage>(this);
	}

	template void System::UnregisterFromUpdate<SimulationStage::FrameStart >();
	template void System::UnregisterFromUpdate<SimulationStage::PrePhysics >();
	template void System::UnregisterFromUpdate<SimulationStage::Physics    >();
	template void System::UnregisterFromUpdate<SimulationStage::PostPhysics>();
	template void System::UnregisterFromUpdate<SimulationStage::FrameEnd   >();

	void System::Deactivate()
	{
		if (!m_Active)
			FE_LOG_CORE_WARN("{0} already deactivated", GetName());

		m_Active = false;
		OnDeactivate();

		UnregisterFromUpdate<SimulationStage::Physics    >();
		UnregisterFromUpdate<SimulationStage::PostPhysics>();
		UnregisterFromUpdate<SimulationStage::PrePhysics >();
		UnregisterFromUpdate<SimulationStage::FrameStart >();
		UnregisterFromUpdate<SimulationStage::FrameEnd   >();
	}

	void System::Shutdown()
	{
		OnShutdown();
	}

}