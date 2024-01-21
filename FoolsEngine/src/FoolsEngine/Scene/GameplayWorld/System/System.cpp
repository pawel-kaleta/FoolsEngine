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
			FE_LOG_CORE_WARN("{0} already active", GetSystemName());

		m_Active = true;
		OnActivate();
	}

	void System::Deactivate()
	{
		if (!m_Active)
			FE_LOG_CORE_WARN("{0} already deactivated", GetSystemName());

		m_Active = false;
		OnDeactivate();

		UnregisterFromUpdate<SimulationStages::Physics    >();
		UnregisterFromUpdate<SimulationStages::PostPhysics>();
		UnregisterFromUpdate<SimulationStages::PrePhysics >();
		UnregisterFromUpdate<SimulationStages::FrameStart >();
		UnregisterFromUpdate<SimulationStages::FrameEnd   >();
	}

	void System::Shutdown()
	{
		OnShutdown();
	}

	void System::Deserialize(YAML::Node& data, GameplayWorld* world)
	{
		FE_LOG_CORE_ERROR("{0} serialization not implemented!", GetSystemName());
	}
}