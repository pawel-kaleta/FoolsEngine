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

    void Behavior::DrawEntity(Entity entity, const std::string& name)
    {
        //placeholder implementation
        EntityID entityID = entity.ID();
        std::string entity_ID_and_name = std::to_string(entityID);
        if (entity.IsHead())
        {
            entity_ID_and_name += " [" + entity.Get<CEntityName>().EntityName + "]";
        }
        else
        {
            auto& head = entity.Get<CHeadEntity>().HeadEntity;

            entity_ID_and_name += " [" + Entity(head, entity.GetWorld()).Get<CEntityName>().EntityName + "]";

            entity_ID_and_name += " " + entity.Get<CEntityName>().EntityName;
        }


        ImGui::BeginDisabled();
        ImGui::DragInt(name.c_str(), (int*)&entityID);
        ImGui::EndDisabled();

    }
}