#pragma once

#include "FoolsEngine\Core\UUID.h"

#include "FoolsEngine\Scene\SimulationStage.h"

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

		virtual std::string GetName() const { return "BaseSystem"; }
		UUID GetUUID() const { return m_UUID; }
		bool IsActive() const { return m_Active; }
		virtual void Serialize(YAML::Emitter& emitter) const { FE_LOG_CORE_ERROR("{0} serialization not implemented!", GetName()); }
		virtual void Deserialize(YAML::Node& data, GameplayWorld* world) { FE_LOG_CORE_ERROR("{0} deserialization not implemented!", GetName()); }
		static std::string GetNameStatic() { return "BaseSystem"; }

	protected:
		virtual void OnInitialize() {};
		virtual void OnActivate() {};

#define _SYSTEM_ON_UPDATE_DECLARATION(x) virtual void OnUpdate_##x() {};
		FE_FOR_EACH(_SYSTEM_ON_UPDATE_DECLARATION, FE_SIMULATION_STAGES);

		virtual void OnDeactivate() {};
		virtual void OnShutdown() {};
		
		template<SimulationStage::ValueType stage>
		void RegisterForUpdate(uint32_t priority);

		template<SimulationStage::ValueType stage>
		void UnregisterFromUpdate();

	private:
		UUID m_UUID;
		SystemsDirector* m_SystemsDirector;
		bool m_Active = false;

		friend class SystemsDirector;
		friend class SceneSerializerYAML;
	};

#define FE_SYSTEM_SETUP(type, name) \
	static std::string GetNameStatic() { return name; } \
	virtual std::string GetName() const override { return name; }
}