#pragma once

#include "FoolsEngine/Application/UUID.h"

#include "FoolsEngine/Scene/SimulationStage.h"

#include "FoolsEngine/Foundation/Memory/Pile.h"

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

		virtual String GetName() const { return String("BaseSystem", 11); }

		virtual void Serialize(YAML::Emitter& emitter) const { Pile p; FE_LOG_CORE_ERROR("{0} serialization not implemented!", GetName().GetCString(&p).Data); }
		virtual void Deserialize(YAML::Node& data, GameplayWorld* world) { Pile p; FE_LOG_CORE_ERROR("{0} deserialization not implemented!", GetName().GetCString(&p).Data); }
		static String GetNameStatic() { return String("BaseSystem", 11); }


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