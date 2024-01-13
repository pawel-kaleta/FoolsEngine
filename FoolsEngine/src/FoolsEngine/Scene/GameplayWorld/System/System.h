#pragma once

#include <string>

namespace fe
{
	class SystemsDirector;

	class System
	{
	public:
		System() = default;
		virtual ~System() = default;

		virtual void OnInitialize() {};

		virtual void OnUpdate_FrameStart()  {};
		virtual void OnUpdate_PrePhysics()  {};
		virtual void OnUpdate_Physics()     {};
		virtual void OnUpdate_PostPhysics() {};
		virtual void OnUpdate_FrameEnd()    {};

		virtual void OnShutdown() {};

		virtual void DrawInspectorWidget() {};

		const std::string& GetName() const { return m_Name; }
		void SetName(std::string& name) { m_Name = name; }

	protected:
		template<typename tnSimulationStage>
		void RegisterForUpdate(uint32_t priority)
		{
			FE_PROFILER_FUNC();

			FE_LOG_CORE_DEBUG("System Update Register");

			void (System::* onUpdateFuncPtr)() = nullptr;
			if (std::is_same_v<tnSimulationStage, SimulationStages::FrameStart	>) onUpdateFuncPtr = &System::OnUpdate_FrameStart;
			if (std::is_same_v<tnSimulationStage, SimulationStages::PrePhysics	>) onUpdateFuncPtr = &System::OnUpdate_PrePhysics;
			if (std::is_same_v<tnSimulationStage, SimulationStages::Physics		>) onUpdateFuncPtr = &System::OnUpdate_Physics;
			if (std::is_same_v<tnSimulationStage, SimulationStages::PostPhysics	>) onUpdateFuncPtr = &System::OnUpdate_PostPhysics;
			if (std::is_same_v<tnSimulationStage, SimulationStages::FrameEnd	>) onUpdateFuncPtr = &System::OnUpdate_FrameEnd;

			FE_CORE_ASSERT(onUpdateFuncPtr, "Did not recognise Simulation Stage!");

			m_SystemsDirector->EnrollForUpdate<tnSimulationStage>(this, onUpdateFuncPtr, priority);
		}

	private:
		UUID m_UUID;
		SystemsDirector* m_SystemsDirector;
		std::string m_Name = "System";

		friend class SystemsDirector;
	};
}