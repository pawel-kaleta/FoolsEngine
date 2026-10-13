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
        {
            Pile p;
            FE_LOG_CORE_WARN("{0} already active", GetBehaviorName().GetCString(&p).Data);
        }

        m_Active = true;
        OnActivate();
    }

    void Behavior::Deactivate()
    {
        if (!m_Active)
        {
            Pile p;
            FE_LOG_CORE_WARN("{0} already deactivated", GetBehaviorName().GetCString(&p).Data);
        }

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

        void (Behavior:: * on_update_func_ptr)() = nullptr;

#define _GET_ON_UPDATE_FUNK_PTR(x) if constexpr (stage == SimulationStage::x) on_update_func_ptr = &Behavior::OnUpdate_##x;
        FE_FOR_EACH(_GET_ON_UPDATE_FUNK_PTR, FE_SIMULATION_STAGES);

        FE_CORE_ASSERT(on_update_func_ptr, "Did not recognise Simulation Stage!");

        Actor(m_HeadEntity).EnrollForUpdate<stage>(this, on_update_func_ptr, priority);
    }

#define _BEHAVIOR_REGISTER_FOR_UPDATE_DEF(x) template void Behavior::RegisterForUpdate<SimulationStage::x>(uint32_t priority);
    FE_FOR_EACH(_BEHAVIOR_REGISTER_FOR_UPDATE_DEF, FE_SIMULATION_STAGES)

    bool Behavior::DrawEntity(Entity& entity, String name)
    {
        Pile p;

        StringBuilder entity_ID_and_name;
        entity_ID_and_name.Init(&p);

        CString name_c = name.GetCString(&p);

        bool was_entity_null = false;
        if (entity)
        {
            entity_ID_and_name.Append(std::to_string(entity.ID()).c_str()); // to do: write our own to_string
            entity_ID_and_name.Append(" ", 2);
            entity_ID_and_name.Append(entity.Get<CEntityName>().EntityName);
        }
        else
        {
            was_entity_null = true;
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.25f,0.25f,0.25f,1.0f });
        }
        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FrameBorderSize, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
        ImGui::PushID(name_c.Data);
        bool selected = ImGui::Button(entity_ID_and_name.GetCString(&p).Data, {ImGui::GetContentRegionAvail().x / 2, ImGui::GetTextLineHeightWithSpacing()});
        ImGui::PopID();
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
        ImGui::Text(name_c.Data);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        if (was_entity_null)
            ImGui::PopStyleColor();

        return selected;
    }
}