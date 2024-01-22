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

        UnregisterFromUpdate<SimulationStages::Stages::Physics    >();
        UnregisterFromUpdate<SimulationStages::Stages::PostPhysics>();
        UnregisterFromUpdate<SimulationStages::Stages::PrePhysics >();
        UnregisterFromUpdate<SimulationStages::Stages::FrameStart >();
        UnregisterFromUpdate<SimulationStages::Stages::FrameEnd   >();
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