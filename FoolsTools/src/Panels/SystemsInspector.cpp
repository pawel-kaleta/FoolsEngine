#include "SystemsInspector.h"

#include "imgui_internal.h"

namespace fe
{
    void SystemsInspector::SetScene(const Ref<Scene>& scene)
    {
        m_Scene = scene;
    }

    void SystemsInspector::OnImGuiRender()
    {
        FE_PROFILER_FUNC();

        ImGui::Begin("Systems Inspector");

        if (m_Scene == nullptr)
        {
            ImGui::End();
            return;
        }

        m_SystemToRemove = nullptr;

        auto& systemsDirector = m_Scene->GetGameplayWorld()->GetSystems();

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Systems"))
            {
                if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
                {
                    if (ImGui::IsMouseClicked(1))
                    {
                        ImGui::OpenPopup("AddSystem");
                    }
                }

                AddSystemPopupMenu();

                auto& systems = systemsDirector.m_Systems;
                int uniqueIdWrap = 0;
                for (const auto& system : systems)
                {
                    ImGui::PushID(++uniqueIdWrap);
                    DrawSystemWidget(system.get());
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
                        auto& updateEnrolls = systemsDirector.m_SystemUpdateEnrolls[stage];
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
                            auto& sys = updateEnroll.System;

                            if (ImGui::InputInt(sys->GetName().c_str(), (int*)&updateEnroll.Priority))
                            {
                                systemsDirector.SortSystemUpdateEnrolls((SimulationStages::Stages)stage);
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

        if (m_SystemToRemove)
        {
            m_Scene->GetGameplayWorld()->GetSystems().RemoveSystem(m_SystemToRemove);
            m_SystemToRemove = nullptr;
        }

        ImGui::End();
    }

    void SystemsInspector::DrawSystemWidget(System* system)
    {
        FE_PROFILER_FUNC();

        auto& name = system->GetName();
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

        ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_ClipLabelForTrailingButton;
        bool widget_open = ImGui::CollapsingHeader(name.c_str(), header_flags);

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - lineHeight + 12.0f); // 12 may be windowPadding + framePadding ?

        static bool popup_open;
        // popup_open is common for all widgets, so wee need to check if it is applicable to this particular widget
        static System* system_of_popup = nullptr;
        bool widget_of_popup = system_of_popup == system;

        ImGuiDir button_arrow_dir = popup_open && widget_of_popup ? ImGuiDir_::ImGuiDir_Down : ImGuiDir_::ImGuiDir_Right;
        bool open_new_popup = false;
        if (ImGui::ArrowButtonEx("settings", button_arrow_dir, ImVec2(lineHeight, lineHeight)))
        {
            system_of_popup = system;
            widget_of_popup = true;
            open_new_popup = true;
        }
        else
        {
            if (system_of_popup)
            {
                popup_open = false;
            }
        }

        if (widget_open)
        {
            system->DrawInspectorWidget();
        }

        if (widget_of_popup)
        {
            if (popup_open || open_new_popup)
                ImGui::OpenPopup("SystemSettings");
        }

        bool removeSystem = false;
        if (ImGui::BeginPopup("SystemSettings"))
        {
            popup_open = true;
            if (ImGui::MenuItem("Remove System"))
            {
                removeSystem = true;
            }

            ImGui::EndPopup();
        }


        if (removeSystem)
        {
            //FE_LOG_CORE_ERROR("Behavior removal not yet implemented");
            m_SystemToRemove = system;
        }
    }

    void SystemsInspector::AddSystemPopupMenu()
    {
        if (ImGui::BeginPopup("AddSystem"))
        {
            //auto& behReg = BehaviorsRegistry::s_Instance;
            //for (const auto& item : behReg.Items)
            //{
            //    auto& getName = item.Name;
            //    std::string name = (behReg.*getName)();
            //    if (ImGui::MenuItem(name.c_str()))
            //    {
            //        auto& create = item.Create;
            //        (actor.*create)();
            //    }
            //}

            ImGui::EndPopup();
        }
    }
}