#include "FE_pch.h"
#include "System.h"

#include "FoolsEngine\Scene\SimulationStages.h"
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

	template<SimulationStages::Stages stage>
	void System::RegisterForUpdate(uint32_t priority)
	{
		FE_PROFILER_FUNC();

		FE_LOG_CORE_DEBUG("System Update Register");

		void (System:: * onUpdateFuncPtr)() = nullptr;

		if constexpr (stage == SimulationStages::Stages::FrameStart) onUpdateFuncPtr = &System::OnUpdate_FrameStart;
		if constexpr (stage == SimulationStages::Stages::PrePhysics) onUpdateFuncPtr = &System::OnUpdate_PrePhysics;
		if constexpr (stage == SimulationStages::Stages::Physics) onUpdateFuncPtr = &System::OnUpdate_Physics;
		if constexpr (stage == SimulationStages::Stages::PostPhysics) onUpdateFuncPtr = &System::OnUpdate_PostPhysics;
		if constexpr (stage == SimulationStages::Stages::FrameEnd) onUpdateFuncPtr = &System::OnUpdate_FrameEnd;

		FE_CORE_ASSERT(onUpdateFuncPtr, "Did not recognise Simulation Stage!");

		m_SystemsDirector->EnrollForUpdate<stage>(this, onUpdateFuncPtr, priority);
	}

	template void System::RegisterForUpdate<SimulationStages::Stages::FrameStart >(uint32_t);
	template void System::RegisterForUpdate<SimulationStages::Stages::PrePhysics >(uint32_t);
	template void System::RegisterForUpdate<SimulationStages::Stages::Physics    >(uint32_t);
	template void System::RegisterForUpdate<SimulationStages::Stages::PostPhysics>(uint32_t);
	template void System::RegisterForUpdate<SimulationStages::Stages::FrameEnd   >(uint32_t);

	template<SimulationStages::Stages stage>
	void System::UnregisterFromUpdate()
	{
		FE_PROFILER_FUNC();

		FE_LOG_CORE_DEBUG("Behavior Update Unregister");

		m_SystemsDirector->RemoveUpdateEnroll<stage>(this);
	}

	template void System::UnregisterFromUpdate<SimulationStages::Stages::FrameStart >();
	template void System::UnregisterFromUpdate<SimulationStages::Stages::PrePhysics >();
	template void System::UnregisterFromUpdate<SimulationStages::Stages::Physics    >();
	template void System::UnregisterFromUpdate<SimulationStages::Stages::PostPhysics>();
	template void System::UnregisterFromUpdate<SimulationStages::Stages::FrameEnd   >();

	void System::Deactivate()
	{
		if (!m_Active)
			FE_LOG_CORE_WARN("{0} already deactivated", GetName());

		m_Active = false;
		OnDeactivate();

		UnregisterFromUpdate<SimulationStages::Stages::Physics    >();
		UnregisterFromUpdate<SimulationStages::Stages::PostPhysics>();
		UnregisterFromUpdate<SimulationStages::Stages::PrePhysics >();
		UnregisterFromUpdate<SimulationStages::Stages::FrameStart >();
		UnregisterFromUpdate<SimulationStages::Stages::FrameEnd   >();
	}

	void System::Shutdown()
	{
		OnShutdown();
	}

}