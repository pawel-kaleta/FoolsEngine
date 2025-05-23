#include "ActorInspector.h"

#include "imgui_internal.h"

#include <glm\gtc\type_ptr.hpp>

namespace fe
{
    void ActorInspector::SetScene(const AssetHandle<Scene>& scene)
    {
        m_Scene = scene;
        m_OpenedActorID = NullEntityID;
    }

    void ActorInspector::OpenActor(EntityID entityID)
    {
        m_SelectedEntityID = entityID;
        if (entityID == NullEntityID)
        {
            m_OpenedActorID = NullEntityID;
            return;
        }

        auto scene_observer = m_Scene.Observe();
        m_OpenedActorID = scene_observer.GetDataComponent().GameplayWorld->GetRegistry().get<CHeadEntity>(entityID).HeadEntity;
    }
    
    void ActorInspector::DrawBehaviorWidget(Behavior* behavior, Actor actor)
    {
        FE_PROFILER_FUNC();

        auto name = behavior->GetBehaviorName();

        ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_ClipLabelForTrailingButton;
        bool widget_open = ImGui::CollapsingHeader(name.c_str(), header_flags);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - lineHeight + 12.0f); // 12 may be windowPadding + framePadding ?

        static bool popup_open;
        // popup_open is common for all widgets, so wee need to check if it is applicable to this particular widget
        static Behavior* behavior_of_popup = nullptr;
        bool widget_of_popup = behavior_of_popup == behavior;

        ImGuiDir button_arrow_dir = popup_open && widget_of_popup ? ImGuiDir_::ImGuiDir_Down : ImGuiDir_::ImGuiDir_Right;
        bool open_new_popup = false;
        if (ImGui::ArrowButtonEx("settings", button_arrow_dir, ImVec2(lineHeight, lineHeight)))
        {
            behavior_of_popup = behavior;
            widget_of_popup = true;
            open_new_popup = true;
        }
        else
        {
            if (widget_of_popup)
            {
                popup_open = false;
            }
        }

        if (widget_open)
        {
            EntityID newSelection = behavior->DrawInspectorWidget();
            if (newSelection != NullEntityID)
                m_EntityIDSelectionRequest = newSelection;
        }

        if (widget_of_popup)
        {
            if (popup_open || open_new_popup)
                ImGui::OpenPopup("BehaviorSettings");
        }

        bool removeBehavior = false;
        if (ImGui::BeginPopup("BehaviorSettings"))
        {
            popup_open = true;
            if (ImGui::MenuItem("Remove behavior"))
                removeBehavior = true;

            ImGui::EndPopup();
        }

        if (removeBehavior)
        {
            //FE_LOG_CORE_ERROR("Behavior removal not yet implemented");
            m_BehaviorToRemove = behavior;
        }
    }

	void ActorInspector::OnImGuiRender()
	{
        FE_PROFILER_FUNC();

        ImGui::Begin("Actor Inspector");
        m_EntityIDSelectionRequest = NullEntityID;

        if (m_OpenedActorID == NullEntityID || m_Scene.GetID() == NullAssetID)
        {
            ImGui::End();
            return;
        }

        m_BehaviorToRemove = nullptr;
        auto scene_observer = m_Scene.Observe();
        Actor actor(m_OpenedActorID, scene_observer.GetDataComponent().GameplayWorld.get());

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Behaviors"))
            {
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);

                DrawCNameWidget(actor);

                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - (lineHeight * 2.5f) + 12.0f); // 12 may be windowPadding + framePadding ?

                if (ImGui::Button("Add +", ImVec2(lineHeight * 2.5f, lineHeight)))
                    ImGui::OpenPopup("AddBehavior");

                AddBehaviorPopupMenu(actor);

                auto& behaviors = actor.m_Data.Get()->m_Behaviors;
                int uniqueIdWrap = 0;
                for (const auto& behavior : behaviors)
                {
                    ImGui::PushID(++uniqueIdWrap);
                    DrawBehaviorWidget(behavior.get(), actor);
                    ImGui::PopID();
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Updates"))
            {
                for (int stage = 0; stage < SimulationStages::Count; stage++)
                {
                    if (ImGui::CollapsingHeader(SimulationStages::Names[stage], ImGuiTreeNodeFlags_None))
                    {
                        auto& updateEnrolls = actor.m_Data.Get()->m_UpdateEnrolls[stage];
                        if (updateEnrolls.size() == 0)
                        {
                            ImGui::Text("None");
                            continue;
                        }

                        ImGui::PushItemWidth(75.0f);

                        int uniqueIdWrap = 0;
                        for (auto& updateEnroll : updateEnrolls)
                        {
                            ImGui::PushID(++uniqueIdWrap);
                            auto& beh = updateEnroll.Behavior;

                            if (ImGui::InputInt(beh->GetBehaviorName().c_str(), (int*)&updateEnroll.Priority))
                            {
                                actor.SortUpdateEnrolls((SimulationStages::Stages)stage);
                            }
                            ImGui::PopID();
                        }
                        ImGui::PopItemWidth();
                    }
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();

        if (m_BehaviorToRemove)
        {
            actor.RemoveBehavior(m_BehaviorToRemove);
            m_BehaviorToRemove = nullptr;
        }
	}

    void ActorInspector::AddBehaviorPopupMenu(Actor actor)
    {
        if (ImGui::BeginPopup("AddBehavior"))
        {
            for (const auto& item : BehaviorsRegistry::GetAll())
            {
                auto& getName = item.GetName;
                std::string name = (*getName)();
                if (ImGui::MenuItem(name.c_str()))
                {
                    auto& create = item.Create;
                    Behavior* behavior = (actor.*create)();
                    behavior->Initialize();
                    behavior->Activate();
                }
            }

            ImGui::EndPopup();
        }
    }

    void ActorInspector::DrawCNameWidget(Entity entity)
    {
        FE_PROFILER_FUNC();

        auto& name = entity.Get<CEntityName>();
        static char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strncpy_s(buffer, name.EntityName.c_str(), sizeof(buffer));
        if (ImGui::InputText("Name", buffer, sizeof(buffer)))
        {
            name.EntityName = std::string(buffer);
        }
    }
}