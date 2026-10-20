#pragma once

#include "FoolsEngine/Application/UUID.h"

#include "FoolsEngine/Scene/SimulationStage.h"

#include "FoolsEngine/Foundation/Context/Pile.h"
#include "FoolsEngine/Foundation/Containers/StringBuilder.h"


namespace YAML
{
	class Emitter;
	class Node;
}

namespace fe
{
	class SystemsDirector;
	class GameplayWorld;

	class System
	{
	public:
		System() = default;
		virtual ~System() = default;

		void Initialize();
		void Activate();
		void Deactivate();
		void Shutdown();

		virtual void DrawInspectorWidget() {};

		virtual String GetName() const { return String("BaseSystem"_FE); }

		virtual void Serialize(YAML::Emitter& emitter) const { Context::Log::Core.Error("{0} serialization not implemented!"_FE, GetName()); }
		virtual void Deserialize(YAML::Node& data, GameplayWorld* world) { Context::Log::Core.Error("{0} deserialization not implemented!"_FE, GetName()); }
		static String GetNameStatic() { return String("BaseSystem"_FE); }


		template<SimulationStage::ValueType stage>
		void RegisterForUpdate(uint32_t priority);

		template<SimulationStage::ValueType stage>
		void UnregisterFromUpdate();


		UUID m_UUID;
		SystemsDirector* m_SystemsDirector;
		bool m_Active = false;

	protected:
		friend class SystemsDirector;

		virtual void OnInitialize() {};
		virtual void OnActivate() {};

#define _SYSTEM_ON_UPDATE_DECLARATION(x) virtual void OnUpdate_##x() {};
		FE_FOR_EACH(_SYSTEM_ON_UPDATE_DECLARATION, FE_SIMULATION_STAGES);

		virtual void OnDeactivate() {};
		virtual void OnShutdown() {};
	};

#define FE_SYSTEM_SETUP(type, name) \
	static String GetNameStatic() { return String(name, sizeof(name)); } \
	virtual String GetName() const override { return String(name, sizeof(name)); }
}