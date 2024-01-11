#pragma once

#include <string>

namespace fe
{
	class GameplayWorld;

	class System
	{
	public:
		System() = default;
		virtual ~System() = default;

		virtual void OnInitialize() {};

		virtual void OnUpdate_FrameStart() {};
		virtual void OnUpdate_PrePhysics() {};
		virtual void OnUpdate_Physics() {};
		virtual void OnUpdate_PostPhysics() {};
		virtual void OnUpdate_FrameEnd() {};

		virtual void OnShutdown() {};

		virtual void DrawInspectorWidget() const {};

		virtual std::string GetName() const { return ""; }

	protected:
		template<typename tnSimulationStage>
		void RegisterForUpdate()
		{
			FE_PROFILER_FUNC();

			FE_LOG_CORE_DEBUG("System Update Register");

			void (System:: * onUpdateFuncPtr)() = nullptr;
			if (std::is_same_v<tnSimulationStage, SimulationStages::FrameStart	>) onUpdateFuncPtr = &Behavior::OnUpdate_FrameStart;
			if (std::is_same_v<tnSimulationStage, SimulationStages::PrePhysics	>) onUpdateFuncPtr = &Behavior::OnUpdate_PrePhysics;
			if (std::is_same_v<tnSimulationStage, SimulationStages::Physics		>) onUpdateFuncPtr = &Behavior::OnUpdate_Physics;
			if (std::is_same_v<tnSimulationStage, SimulationStages::PostPhysics	>) onUpdateFuncPtr = &Behavior::OnUpdate_PostPhysics;
			if (std::is_same_v<tnSimulationStage, SimulationStages::FrameEnd	>) onUpdateFuncPtr = &Behavior::OnUpdate_FrameEnd;

			FE_CORE_ASSERT(onUpdateFuncPtr, "Did not recognise Simulation Stage!");

			RegisterInWorld<tnSimulationStage>(onUpdateFuncPtr);
		}

	private:
		UUID m_UUID;
		GameplayWorld* m_GameplayWorld;

		template <typename tnSimulationStage>
		void RegisterInWorld(void (System::* onUpdateFuncPtr)())
		{
			m_GameplayWorld->EnrollForUpdate<tnSimulationStage>(this, onUpdateFuncPtr);
		}
	};
}