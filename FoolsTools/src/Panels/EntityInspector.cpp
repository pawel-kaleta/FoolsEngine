#include "EntityInspector.h"

#include "imgui_internal.h"

namespace fe
{
    void EntityInspector::SetScene(const AssetHandle<Scene>& scene)
    {
        m_Scene = scene;
        m_OpenedEntityID = NullEntityID;
    }

    void EntityInspector::OpenEntity(EntityID entityID)
    {
        m_OpenedEntityID = entityID;
    }

    void EntityInspector::DrawComponentWidget(const ComponentTypesRegistry::DataComponentRegistryItem& item, BaseEntity entity)
    {
        FE_PROFILER_FUNC();

        auto& getter = item.Getter;
        DataComponent* component = (entity.*getter)();

        if (component == nullptr)
            return;

        ImGui::PushID(component);
        auto name = component->GetName();

        ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_ClipLabelForTrailingButton;
        bool widget_open = ImGui::CollapsingHeader(name.c_str(), header_flags);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - lineHeight + 12.0f); // 12 may be windowPadding + framePadding ?

        static bool popup_open;
        // popup_open is common for all widgets, so wee need to check if it is applicable to this particular widget
        // simply component* is not good, because it is not stable across frames
        static DataComponent* (BaseEntity::* getter_of_component_of_popup)() const = nullptr;
        bool widget_of_popup = getter_of_component_of_popup == getter;

        ImGuiDir button_arrow_dir = widget_of_popup && popup_open ? ImGuiDir_::ImGuiDir_Down : ImGuiDir_::ImGuiDir_Right;
        bool open_new_popup = false;
        if (ImGui::ArrowButtonEx("settings", button_arrow_dir, ImVec2(lineHeight, lineHeight)))
        {
            open_new_popup = true;
            getter_of_component_of_popup = getter;
            widget_of_popup = true;
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
            if (component->IsSpatial())
            {
                if (ImGui::TreeNodeEx("Offset", ImGuiTreeNodeFlags_SpanAvailWidth))
                {
                    auto& offset = ((SpatialComponent*)component)->Offset;
                    ImGui::DragFloat3("Shift", glm::value_ptr(offset.Shift), 0.01f, 0, 0, "%.2f");
                    ImGui::DragFloat3("Rotation", glm::value_ptr(offset.Rotation), 0.10f, 0, 0, "%.2f");
                    ImGui::DragFloat3("Scale", glm::value_ptr(offset.Scale), 0.01f, 0, 0, "%.2f");
                    ImGui::TreePop();
                }
                ImGui::Separator();
            }
            ImGui::TreePush("Comp data");
            component->DrawInspectorWidget(entity);
            ImGui::TreePop();
            
        }

        if (widget_of_popup)
        {
            if (popup_open || open_new_popup)
                ImGui::OpenPopup("ComponentSettings");
        }

        bool removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings"))
        {
            popup_open = true;
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

        ImGui::PopID();
    }

    void EntityInspector::DrawComponentsTab(Entity entity)
    {
        if (!ImGui::BeginTabItem("Components"))
        {
            return;
        }

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);

        DrawCNameWidget(entity);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - (lineHeight * 2.5f) + 12.0f); // 12 may be windowPadding + framePadding ?

        if (ImGui::Button("Add +", ImVec2(lineHeight * 2.5f, lineHeight)))
            ImGui::OpenPopup("AddComponent");

        AddComponentPopupMenu(entity);

        DrawCTransformWidget(entity);

        for (const auto& item : ComponentTypesRegistry::GetDataCompItems())
        {
            DrawComponentWidget(item, entity);
        }

        ImGui::EndTabItem();
    }

    void EntityInspector::OnImGuiRender()
    {
        FE_PROFILER_FUNC();

        ImGui::Begin("Entity Inspector");

        if (m_OpenedEntityID == NullEntityID || m_Scene.GetID() == NullAssetID)
        {
            ImGui::End();
            return;
        }

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (!ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            ImGui::End();
            return;
        }

        auto scene_Observer = m_Scene.Observe();
        Entity entity(m_OpenedEntityID, scene_Observer.GetWorlds().GameplayWorld.get());

        DrawComponentsTab(entity);
        
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
            DrawDebugTab(entity);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
        ImGui::End();
    }

    void EntityInspector::AddComponentPopupMenu(BaseEntity entity)
    {
        if (!ImGui::BeginPopup("AddComponent")) return;

        for (const auto& item : ComponentTypesRegistry::GetDataCompItems())
        {
            auto& getter = item.Getter;
            DataComponent* component = (entity.*getter)();

            if (!component)
            {
                auto& getName = item.GetName;
                std::string name = (*getName)();
                if (ImGui::MenuItem(name.c_str()))
                {
                    auto& emplacer = item.Emplacer;
                    (entity.*emplacer)();
                }
            }
        }

        ImGui::EndPopup();
    }

    void EntityInspector::DrawCNameWidget(Entity entity)
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
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {

            // Set payload to carry the index of our item (could be anything)
            ImGui::SetDragDropPayload("Entity", &entity, sizeof(entity));

            // Display preview (could be anything, e.g. when dragging an image we could decide to display
            // the filename and a small preview of the image, etc.)
            
            ImGui::Text(entity.GetNameSignature().c_str());

            ImGui::EndDragDropSource();
        }
    }

    void EntityInspector::DrawCTransformWidget(Entity entity)
    {
        FE_PROFILER_FUNC();

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
        {
            auto transform = entity.GetTransformHandle().Local();

            bool change = false;

            change = change || ImGui::DragFloat3("Shift", glm::value_ptr(transform.Shift), 0.01f, 0, 0, "%.2f");
            change = change || ImGui::DragFloat3("Rotation", glm::value_ptr(transform.Rotation), 0.10f, 0, 0, "%.2f");
            change = change || ImGui::DragFloat3("Scale", glm::value_ptr(transform.Scale), 0.01f, 0, 0, "%.2f");

            if (change)
                entity.GetTransformHandle().SetLocal(transform);
        }
    }

    void EntityInspector::DrawCTagsWidget(Entity entity)
    {
        FE_PROFILER_FUNC();

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

        if (ImGui::BeginTable("split", 5, flags))
        {
            uint32_t column_flags = ImGuiTableColumnFlags_AngledHeader | ImGuiTableColumnFlags_WidthFixed;
            
            ImGui::TableSetupScrollFreeze(0, 2); // Make top row2 always visible
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 19.0f);
            ImGui::TableSetupColumn("Parent", column_flags, 19.0f);
            ImGui::TableSetupColumn("Local", column_flags, 19.0f);
            ImGui::TableSetupColumn("Global", column_flags, 19.0f);
            ImGui::TableSetupColumn("Tag Name");
            ImGui::TableAngledHeadersRow();
            ImGui::TableHeadersRow();

            for (int i = 0; i < 64; i++)
            {
                ImGui::PushID(i);
                
                ImGui::TableNextColumn();
                auto id_str = (i < 10 ? " " : "") + std::to_string(i);
                ImGui::Text(id_str.c_str());

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
                ImGui::Text(tagLabels[i]);

                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::PopID();

        tagsHandle.SetLocal(tagsLocal);
    }

    void EntityInspector::DrawDebugTab(Entity entity)
    {
        FE_PROFILER_FUNC();

        if (ImGui::CollapsingHeader("Node Component", ImGuiTreeNodeFlags_None))
        {
            auto& node = entity.Get<CEntityNode>();

            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);

            ImGui::BeginDisabled();
            ImGui::DragInt("Parent", (int*)&node.Parent);
            ImGui::DragInt("HierarchyLvl", (int*)&node.HierarchyLvl);
            ImGui::DragInt("ChildrenCount", (int*)&node.ChildrenCount);
            ImGui::DragInt("FirstChild", (int*)&node.FirstChild);
            ImGui::DragInt("NextSibling", (int*)&node.NextSibling);
            ImGui::DragInt("PreviousSibling", (int*)&node.PreviousSibling);
            ImGui::EndDisabled();

            ImGui::PopItemWidth();
        }
    }
}