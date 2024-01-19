#pragma once

#include "FoolsEngine\Core\UUID.h"

#include <string>

#include <yaml-cpp\yaml.h>

namespace fe
{
	class SystemsDirector;
	class GameplayWorld;

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

		virtual std::string GetSystemName() const { return "BaseSystem"; }
		UUID GetUUID() const { return m_UUID; }
		virtual void Serialize(YAML::Emitter& emitter) const { FE_LOG_CORE_ERROR("{0} serialization not implemented!", GetSystemName()); }
		virtual void Deserialize(YAML::Node& data, GameplayWorld* world);
		static std::string GetName() { return "BaseSystem"; }

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

		friend class SystemsDirector;
		friend class SceneSerializerYAML;
	};

#define FE_SYSTEM_SETUP(type, name) \
	static std::string GetName() { return name; } \
	virtual std::string GetSystemName() const override { return name; }
}