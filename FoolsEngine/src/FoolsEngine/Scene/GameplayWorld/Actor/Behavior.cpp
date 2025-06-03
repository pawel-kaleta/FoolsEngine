#include "FE_pch.h"
#include "Behavior.h"
#include "Actor.h"

namespace fe
{
    void Behavior::Initialize()
    {
        OnInitialize();
    }

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

#define _BEHAVIOR_UREGISTER_FROM_UPDATE_CALL(x) UnregisterFromUpdate<SimulationStage::x>();
        FE_FOR_EACH(_BEHAVIOR_UREGISTER_FROM_UPDATE_CALL, FE_SIMULATION_STAGES);
    }

    void Behavior::Shutdown()
    {
        OnShutdown();
    }

    template<SimulationStage::ValueType stage>
    void Behavior::RegisterForUpdate(uint32_t priority)
    {
        FE_PROFILER_FUNC();

        FE_LOG_CORE_DEBUG("Behavior Update Register");

        void (Behavior:: * onUpdateFuncPtr)() = nullptr;

#define _GET_ON_UPDATE_FUNK_PTR(x) if constexpr (stage == SimulationStage::x) onUpdateFuncPtr = &Behavior::OnUpdate_##x;
        FE_FOR_EACH(_GET_ON_UPDATE_FUNK_PTR, FE_SIMULATION_STAGES);

        FE_CORE_ASSERT(onUpdateFuncPtr, "Did not recognise Simulation Stage!");

        Actor(m_HeadEntity).EnrollForUpdate<stage>(this, onUpdateFuncPtr, priority);
    }

#define _BEHAVIOR_REGISTER_FOR_UPDATE_DEF(x) template void Behavior::RegisterForUpdate<SimulationStage::x>(uint32_t priority);
    FE_FOR_EACH(_BEHAVIOR_REGISTER_FOR_UPDATE_DEF, FE_SIMULATION_STAGES);

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