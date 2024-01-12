#include "Inspector.h"

#include <imgui_internal.h>

#include <glm\gtc\type_ptr.hpp>

namespace fe
{
    void Inspector::SetScene(const Ref<Scene>& scene)
    {
        m_Scene = scene;
    }

    void Inspector::OpenEntity(EntityID entityID)
	{
		m_OpenedEntityID = entityID;
	}

    void Inspector::DrawComponentWidget(const ComponentTypesRegistry::DataComponentRegistryItem& item, BaseEntity entity)
    {
        auto& getter = item.Getter;
        DataComponent* component = (entity.*getter)();

        if (component == nullptr)
            return;

        auto name = component->GetComponentName();

        ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_ClipLabelForTrailingButton;
        bool widget_open = ImGui::CollapsingHeader(name.c_str(), header_flags);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - lineHeight + 12.0f); // 12 may be windowPadding + framePadding ?

        static bool settings_button;
        ImGuiDir button_arrow_dir = settings_button ? ImGuiDir_::ImGuiDir_Down : ImGuiDir_::ImGuiDir_Right;
        settings_button = ImGui::ArrowButtonEx("settings", button_arrow_dir, ImVec2(lineHeight, lineHeight));

        if (widget_open)
            component->DrawInspectorWidget(entity);

        if (settings_button)
            ImGui::OpenPopup("ComponentSettings");

        bool removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings"))
        {
            settings_button = true;
            if (ImGui::MenuItem("Remove component"))
                removeComponent = true;

            ImGui::EndPopup();
        }

        if (removeComponent)
        {
            if (entity.GetWorld()->IsGameplayWorld())
            {
                auto destroy = item.DestructionScheduler;
                Entity gamplayEntity(entity);
                (gamplayEntity.*destroy)();
            }
            else
            {
                FE_CORE_ASSERT(false, "Destruction of components from worlds other then GameplayWorld is not yet implemented!");
            }
        }
    }

    void Inspector::DrawBehaviorWidget(Behavior* behavior)
    {
        auto name = behavior->GetBehaviorName();

        ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_ClipLabelForTrailingButton;
        bool widget_open = ImGui::CollapsingHeader(name.c_str(), header_flags);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - lineHeight + 12.0f); // 12 may be windowPadding + framePadding ?

        static bool settings_button;
        ImGuiDir button_arrow_dir = settings_button ? ImGuiDir_::ImGuiDir_Down : ImGuiDir_::ImGuiDir_Right;
        settings_button = ImGui::ArrowButtonEx("settings", button_arrow_dir, ImVec2(lineHeight, lineHeight));

        if (widget_open)
            behavior->DrawInspectorWidget();

        if (settings_button)
            ImGui::OpenPopup("BehaviorSettings");

        bool removeBehavior = false;
        if (ImGui::BeginPopup("BehaviorSettings"))
        {
            settings_button = true;
            if (ImGui::MenuItem("Remove behavior"))
                removeBehavior = true;

            ImGui::EndPopup();
        }

        if (removeBehavior)
        {
            FE_LOG_CORE_ERROR("Behavior removal not yet implemented");
        }
    }

    void Inspector::DrawActorInspector()
    {
        // Most likely gonna crash with Non-GameplayWorld entity selected

        ImGui::Begin("Actor Inspector");

        if (m_OpenedEntityID == NullEntityID || m_Scene == nullptr)
        {
            ImGui::End();
            return;
        }
        
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            EntityID headEntity = m_Scene->GetGameplayWorld()->GetRegistry().get<CHeadEntity>(m_OpenedEntityID).HeadEntity;
            Actor actor(headEntity, m_Scene->GetGameplayWorld());

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
                    DrawBehaviorWidget(behavior.get());
                    ImGui::PopID();
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Updates"))
            {                
                constexpr int stagesCount = (int)SimulationStages::Stages::StagesCount;
                
                for (int stage = 0; stage < stagesCount; stage++)
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
                            if (ImGui::InputInt(updateEnroll.Behavior->GetBehaviorName().c_str(), (int*)&updateEnroll.Priority))
                            {
                                actor.SortUpdateEnrolls(stage);
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
    }

    void Inspector::DrawEntityInspector()
    {
        ImGui::Begin("Entity Inspector");

        if (m_OpenedEntityID == NullEntityID || m_Scene == nullptr)
        {
            ImGui::End();
            return;
        }

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            Entity entity(m_OpenedEntityID, m_Scene->GetGameplayWorld());
            
            if (ImGui::BeginTabItem("Components"))
            {
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);

                DrawCNameWidget(entity);

                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - (lineHeight * 2.5f) + 12.0f); // 12 may be windowPadding + framePadding ?

                if (ImGui::Button("Add +", ImVec2(lineHeight * 2.5f, lineHeight)))
                    ImGui::OpenPopup("AddComponent");

                AddComponentPopupMenu(entity);

                DrawCTransformWidget(entity);

                for (const auto& item : ComponentTypesRegistry::s_Registry.DataItems)
                {
                    DrawComponentWidget(item, entity);
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Tags"))
            {
                DrawCTagsWidget(entity);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Flags"))
            {
                ImGui::Text("UI representation not implemented yet");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Debug"))
            {
                DrawNodeWidget(entity);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }


        

        ImGui::End();
    }

	void Inspector::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

        DrawActorInspector();
        DrawEntityInspector();
	}

    void Inspector::AddComponentPopupMenu(BaseEntity entity)
    {
        if (ImGui::BeginPopup("AddComponent"))
        {
            auto& compReg = ComponentTypesRegistry::s_Registry;
            for (const auto& item : compReg.DataItems)
            {
                auto& getter = item.Getter;
                DataComponent* component = (entity.*getter)();

                if (!component)
                {
                    auto& getName = item.Name;
                    std::string name = (compReg.*getName)();
                    if (ImGui::MenuItem(name.c_str()))
                    {
                        auto& emplacer = item.Emplacer;
                        (entity.*emplacer)();
                    }
                }
            }

            ImGui::EndPopup();
        }
    }

    void Inspector::AddBehaviorPopupMenu(Actor actor)
    {
        if (ImGui::BeginPopup("AddBehavior"))
        {
            auto& behReg = BehaviorsRegistry::s_Registry;
            for (const auto& item : behReg.Items)
            {
                auto& getName = item.Name;
                std::string name = (behReg.*getName)();
                if (ImGui::MenuItem(name.c_str()))
                {
                    auto& create = item.Create;
                    (actor.*create)();
                }
            }

            ImGui::EndPopup();
        }
    }

    void Inspector::DrawCNameWidget(Entity entity)
    {
        auto& name = entity.Get<CEntityName>();
        static char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strncpy_s(buffer, name.EntityName.c_str(), sizeof(buffer));
        if (ImGui::InputText("Name", buffer, sizeof(buffer)))
        {
            name.EntityName = std::string(buffer);
        }
    }

    void Inspector::DrawCTransformWidget(Entity entity)
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
        {
            auto transform = entity.GetTransformHandle().Local();

            ImGui::DragFloat3("Position", glm::value_ptr(transform.Position), 0.01f, 0, 0, "%.2f");
            ImGui::DragFloat3("Rotation", glm::value_ptr(transform.Rotation), 0.10f, 0, 0, "%.2f");
            ImGui::DragFloat3("Scale"   , glm::value_ptr(transform.Scale)   , 0.01f, 0, 0, "%.2f");

            entity.GetTransformHandle().SetLocal(transform);
        }
    }

    void Inspector::DrawCTagsWidget(Entity entity)
    {
        constexpr char* tagLabels[64] = {
            "Error",
            "Player",
            "Very very very very long Tag name test",
            " 3",
            " 4",
            " 5",
            " 6",
            " 7",
            " 8",
            " 9",
            "10",
            "11",
            "12",
            "13",
            "14",
            "15",
            "16",
            "17",
            "18",
            "19",
            "20",
            "21",
            "22",
            "23",
            "24",
            "25",
            "26",
            "27",
            "28",
            "29",
            "30",
            "31",
            "32",
            "33",
            "34",
            "35",
            "36",
            "37",
            "38",
            "39",
            "40",
            "41",
            "42",
            "43",
            "44",
            "45",
            "46",
            "47",
            "48",
            "49",
            "50",
            "51",
            "52",
            "53",
            "54",
            "55",
            "56",
            "57",
            "58",
            "59",
            "60",
            "61",
            "62",
            "63"
        };

        auto tagsHandle = entity.GetTagsHandle();
        auto tagsGlobal = tagsHandle.Global();
        auto tagsLocal = tagsHandle.Local();
        Tags tagsInherited;
        EntityID parent = entity.GetWorld()->GetRegistry().get<CEntityNode>(entity.ID()).Parent;
        if (parent != RootID)
        {
            Entity parentEntity = Entity(parent, entity.GetWorld());
            tagsInherited = parentEntity.GetTagsHandle().Global();
        }

        constexpr ImGuiTableFlags flags =
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersOuter |
            ImGuiTableFlags_BordersV |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_NoSavedSettings;

        ImGui::PushID(1);
        if (ImGui::BeginTable("split", 5, flags))//, ImVec2(0, 150)
        {
            ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
            ImGui::TableSetupColumn("Parent"  , ImGuiTableColumnFlags_WidthFixed, 19.0f);
            ImGui::TableSetupColumn("Local"   , ImGuiTableColumnFlags_WidthFixed, 19.0f);
            ImGui::TableSetupColumn("Global"  , ImGuiTableColumnFlags_WidthFixed, 19.0f);
            ImGui::TableSetupColumn("ID"      , ImGuiTableColumnFlags_WidthFixed, 14.0f);
            ImGui::TableSetupColumn("Tag Name", ImGuiTableColumnFlags_None);
            ImGui::TableHeadersRow();

            for (int i = 0; i < 64; i++)
            {
                ImGui::PushID(i);

                ImGui::TableNextColumn();
                ImGui::BeginDisabled();
                ImGui::CheckboxFlags("##1", &tagsInherited.TagBitFlags, WIDE_BIT_FLAG(i));
                ImGui::EndDisabled();

                ImGui::TableNextColumn();
                ImGui::CheckboxFlags("##2", &tagsLocal.TagBitFlags, WIDE_BIT_FLAG(i));

                ImGui::TableNextColumn();
                ImGui::BeginDisabled();
                ImGui::CheckboxFlags("##3", &tagsGlobal.TagBitFlags, WIDE_BIT_FLAG(i));
                ImGui::EndDisabled();

                ImGui::TableNextColumn();
                auto id_str = (i < 10 ? " " : "") + std::to_string(i);
                ImGui::Text(id_str.c_str());

                ImGui::TableNextColumn();
                ImGui::Text(tagLabels[i]);

                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::PopID();

        tagsHandle.SetLocal(tagsLocal);
    }

    void Inspector::DrawNodeWidget(Entity entity)
    {
        if (ImGui::CollapsingHeader("Node Component", ImGuiTreeNodeFlags_None))
        {
            auto& node = entity.Get<CEntityNode>();

            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);

            ImGui::BeginDisabled();
            ImGui::DragInt("Parent",          (int*)&node.Parent);
            ImGui::DragInt("HierarchyLvl",    (int*)&node.HierarchyLvl);
            ImGui::DragInt("ChildrenCount",   (int*)&node.ChildrenCount);
            ImGui::DragInt("FirstChild",      (int*)&node.FirstChild);
            ImGui::DragInt("NextSibling",     (int*)&node.NextSibling);
            ImGui::DragInt("PreviousSibling", (int*)&node.PreviousSibling);
            ImGui::EndDisabled();

            ImGui::PopItemWidth();
        }
    }
}