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

    bool Behavior::DrawEntity(Entity& entity, const std::string& name)
    {
        std::string entity_ID_and_name;
        bool nullEntity = false;
        if (entity)
        {
            entity_ID_and_name = std::to_string(entity.ID()) + " " + entity.Get<CEntityName>().EntityName;
        }
        else
        {
            nullEntity = true;
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.25f,0.25f,0.25f,1.0f });
        }
        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FrameBorderSize, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
        bool selected = ImGui::Button(entity_ID_and_name.c_str(), {ImGui::GetContentRegionAvail().x / 2, ImGui::GetTextLineHeightWithSpacing()});
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
            {
                IM_ASSERT(payload->DataSize == sizeof(Entity));
                Entity newEntity = *(const Entity*)payload->Data;
                if (newEntity)
                    entity = newEntity;
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::SameLine();
        ImGui::Text(name.c_str());
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        if (nullEntity)
            ImGui::PopStyleColor();

        return selected;
    }
}