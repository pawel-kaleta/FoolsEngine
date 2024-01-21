#include "FE_pch.h"
#include "Behavior.h"
#include "Actor.h"

namespace fe
{
    void Behavior::Activate()
    {
        if (m_Active)
            FE_LOG_CORE_WARN("{0} already active", GetBehaviorName());

        m_Active = true;
        OnActivate();
    }

    void Behavior::Deactivate()
    {
        if (!m_Active)
            FE_LOG_CORE_WARN("{0} already deactivated", GetBehaviorName());

        m_Active = false;
        OnDeactivate();

        UnregisterFromUpdate<SimulationStages::Physics    >();
        UnregisterFromUpdate<SimulationStages::PostPhysics>();
        UnregisterFromUpdate<SimulationStages::PrePhysics >();
        UnregisterFromUpdate<SimulationStages::FrameStart >();
        UnregisterFromUpdate<SimulationStages::FrameEnd   >();
    }

    void Behavior::Initialize()
    {
        OnInitialize();
    }

    void Behavior::Shutdown()
    {
        OnShutdown();
    }
}