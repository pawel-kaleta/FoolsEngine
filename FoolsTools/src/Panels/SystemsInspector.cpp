#include "SystemsInspector.h"

#include "imgui_internal.h"

namespace fe
{
    void SystemsInspector::SetScene(const AssetHandle<Scene>& scene)
    {
        m_Scene = scene;
    }

    void SystemsInspector::OnImGuiRender()
    {
        FE_PROFILER_FUNC();

        ImGui::Begin("Systems Inspector");

        if (m_Scene.GetID() == NullAssetID)
        {
            ImGui::End();
            return;
        }

        m_SystemToRemove = nullptr;

        auto scene_observer = m_Scene.Observe();
        auto& systems_director = scene_observer.GetCoreComponent().GameplayWorld->GetSystems();

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

                auto& systems = systems_director.m_Systems;
                int unique_ImGui_ID_wrap = 0;
                for (const auto& system : systems)
                {
                    ImGui::PushID(++unique_ImGui_ID_wrap);
                    DrawSystemWidget(system.get());
                    ImGui::PopID();
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Updates"))
            {
                for (int i = 0; i < SimulationStage::Count; i++)
                {
                    SimulationStage stage; stage.FromInt(i);
                    if (ImGui::CollapsingHeader(stage.ToConstCharPtr(), ImGuiTreeNodeFlags_None))
                    {
                        auto& update_enrolls = systems_director.m_SystemUpdateEnrolls[i];
                        if (update_enrolls.size() == 0)
                        {
                            ImGui::Text("None");
                            continue;
                        }

                        ImGui::PushItemWidth(75.0f);

                        int unique_ImGui_ID_wrap = 0;
                        for (auto& update_enroll : update_enrolls)
                        {
                            ImGui::PushID(++unique_ImGui_ID_wrap);
                            auto& sys = update_enroll.System;

                            if (ImGui::InputInt(sys->GetName().c_str(), (int*)&update_enroll.Priority))
                            {
                                systems_director.SortSystemUpdateEnrolls(stage);
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
            systems_director.RemoveSystem(m_SystemToRemove);
            m_SystemToRemove = nullptr;
        }

        ImGui::End();
    }

    void SystemsInspector::DrawSystemWidget(System* system)
    {
        FE_PROFILER_FUNC();

        auto name = system->GetName();
        float line_height = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;

        ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_ClipLabelForTrailingButton;
        bool widget_open = ImGui::CollapsingHeader(name.c_str(), header_flags);

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - line_height + 12.0f); // 12 may be windowPadding + framePadding ?

        static bool popup_open;
        // popup_open is common for all widgets, so wee need to check if it is applicable to this particular widget
        static System* system_of_popup = nullptr;
        bool widget_of_popup = system_of_popup == system;

        ImGuiDir button_arrow_dir = popup_open && widget_of_popup ? ImGuiDir::ImGuiDir_Down : ImGuiDir::ImGuiDir_Right;
        bool open_new_popup = false;
        if (ImGui::ArrowButtonEx("settings", button_arrow_dir, ImVec2(line_height, line_height)))
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

        bool should_remove_system = false;
        if (ImGui::BeginPopup("SystemSettings"))
        {
            popup_open = true;
            if (ImGui::MenuItem("Remove System"))
            {
                should_remove_system = true;
            }

            ImGui::EndPopup();
        }

        if (should_remove_system)
        {
            m_SystemToRemove = system;
        }
    }

    void SystemsInspector::AddSystemPopupMenu()
    {
        if (ImGui::BeginPopup("AddSystem"))
        {
            FE_LOG_CORE_WARN("Not implemented");

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