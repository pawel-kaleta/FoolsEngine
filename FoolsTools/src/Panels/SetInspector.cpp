#include "SetInspector.h"


#include <imgui_internal.h>

#include <glm\gtc\type_ptr.hpp>

namespace fe
{
    void SetInspector::SetScene(const Ref<Scene>& scene)
    {
        m_Scene = scene;
    }

    void SetInspector::OpenSet(SetID setID)
	{
		m_OpenedSetID = setID;
	}

    template<typename Component, typename DrawFunc>
    static void DrawComponentWidget(const std::string& name, Set& set, DrawFunc drawFunc)
    {
        auto component = set.GetIfExist<Component>();
        if (component == nullptr)
            return;

        ImGui::PushID(name.c_str());

        ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_ClipLabelForTrailingButton;
        bool widget_open = ImGui::CollapsingHeader(name.c_str(), header_flags);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - lineHeight + 12.0f); // 12 may be windowPadding + framePadding ?

        static bool settings_button;
        ImGuiDir button_arrow_dir = settings_button ? ImGuiDir_::ImGuiDir_Down : ImGuiDir_::ImGuiDir_Right;
        settings_button = ImGui::ArrowButtonEx("settings", button_arrow_dir, ImVec2(lineHeight, lineHeight));
        
        if (widget_open)
            drawFunc(*component, set); // component is a pointer, but drawFunc expects reference

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
            set.Remove<Component>();

        ImGui::PopID();
    }

	void SetInspector::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

		ImGui::Begin("Set Inspector");

        if (m_OpenedSetID == NullSetID || m_Scene == nullptr)
        {
            ImGui::End();
            return;
        }

	    Set set(m_OpenedSetID, m_Scene.get());
        
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);

        DrawCNameWidget(set);
        
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - (lineHeight * 2.5f) + 12.0f); // 12 may be windowPadding + framePadding ?

        if (ImGui::Button("Add +", ImVec2(lineHeight * 2.5f, lineHeight)))
            ImGui::OpenPopup("AddComponent");
        
        AddComponentPopupMenu(set);

        DrawCTransformWidget(set);
        DrawCTagsWidget(set);

        DrawComponentWidget<Renderer2D::CSprite>("Sprite", set, DrawCSpriteWidget);
        DrawComponentWidget<Renderer2D::CTile>("Tile", set, DrawCTileWidget);
        DrawComponentWidget<CCamera>("Camera", set, DrawCCameraWidget);
        
		ImGui::End();
	}

    void SetInspector::AddComponentPopupMenu(Set set)
    {
        if (ImGui::BeginPopup("AddComponent"))
        {
            if (!set.AllOf<CCamera>())
            {
                if (ImGui::MenuItem("Camera"))
                {
                    set.Emplace<CCamera>();
                }
            }

            if (!set.AllOf<Renderer2D::CSprite>())
            {
                if (ImGui::MenuItem("Sprite"))
                {
                    set.Emplace<Renderer2D::CSprite>();
                }
            }

            if (!set.AllOf<Renderer2D::CTile>())
            {
                if (ImGui::MenuItem("Tile"))
                {
                    set.Emplace<Renderer2D::CTile>();
                }
            }

            ImGui::EndPopup();
        }
    }

    void SetInspector::DrawCNameWidget(Set set)
    {
        auto& name = set.Get<CName>();
        static char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        std::strncpy(buffer, name.Name.c_str(), sizeof(buffer));
        if (ImGui::InputText("Name", buffer, sizeof(buffer)))
        {
            name.Name = std::string(buffer);
        }
    }

    void SetInspector::DrawCTransformWidget(Set set)
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
        {
            auto transform = set.GetTransformHandle().Local();

            ImGui::DragFloat3("Position", glm::value_ptr(transform.Position), 0.01f, 0, 0, "%.2f");
            ImGui::DragFloat3("Rotation", glm::value_ptr(transform.Rotation), 0.1f,0,0,"%.2f");
            ImGui::DragFloat3("Scale", glm::value_ptr(transform.Scale), 0.01f, 0, 0, "%.2f");

            set.GetTransformHandle().SetLocal(transform);
        }
    }

    void SetInspector::DrawTagsTable(uint32_t* tagsInherited, uint32_t* tagsLocal, uint32_t* tagsGlobal, const char* const* tagLabels)
    {
        constexpr ImGuiTableFlags flags =
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersOuter |
            ImGuiTableFlags_BordersV |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_NoSavedSettings;

        if (ImGui::BeginTable("split", 5, flags, ImVec2(0, 133)))
        {
            ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
            ImGui::TableSetupColumn("Parent", ImGuiTableColumnFlags_WidthFixed, 19.0f);
            ImGui::TableSetupColumn("Local", ImGuiTableColumnFlags_WidthFixed, 19.0f);
            ImGui::TableSetupColumn("Global", ImGuiTableColumnFlags_WidthFixed, 19.0f);
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 14.0f);
            ImGui::TableSetupColumn("Tag Name", ImGuiTableColumnFlags_None);
            ImGui::TableHeadersRow();

            for (int i = 0; i < 32; i++)
            {
                ImGui::PushID(i);

                ImGui::TableNextColumn();
                ImGui::BeginDisabled();
                ImGui::CheckboxFlags("##1", tagsInherited, BIT_FLAG(i));
                ImGui::EndDisabled();

                ImGui::TableNextColumn();
                ImGui::CheckboxFlags("##2", tagsLocal, BIT_FLAG(i));

                ImGui::TableNextColumn();
                ImGui::BeginDisabled();
                ImGui::CheckboxFlags("##3", tagsGlobal, BIT_FLAG(i));
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
    }

    void SetInspector::DrawCTagsWidget(Set set)
    {
        constexpr char* nodeLabels[] = { "Common", "Internal" };
        constexpr char* commonTagLabel[] = {
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
            "31"
        };
        constexpr char* internalTagLabel[] = {
            " 0",
            " 1",
            " 2",
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
            "31"
        };

        if (ImGui::CollapsingHeader("Tags", ImGuiTreeNodeFlags_None))
        {
            auto tagsGlobal = set.GetTagsHandle().Global();
            auto tagsLocal = set.GetTagsHandle().Local();
            Tags tagsInherited;
            SetID parent = set.GetScene()->GetRegistry().get<CHierarchyNode>(set.ID()).Parent;
            if (parent != RootID)
                tagsInherited = set.GetScene()->GetHierarchy().GetComponent<CTags, Tags>(parent).Global();

            auto flags =
                ImGuiTreeNodeFlags_OpenOnArrow |
                ImGuiTreeNodeFlags_OpenOnDoubleClick |
                ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_NoTreePushOnOpen ;

            if (ImGui::TreeNodeEx((void*)nodeLabels[0], flags, nodeLabels[0]))
            {
                ImGui::PushID(1);
                DrawTagsTable(&tagsInherited.Common.TagBitFlags, &tagsLocal.Common.TagBitFlags, &tagsGlobal.Common.TagBitFlags, commonTagLabel);
                ImGui::PopID();
            }
            
            if (ImGui::TreeNodeEx((void*)nodeLabels[1], flags, nodeLabels[1]))
            {
                ImGui::PushID(2);
                DrawTagsTable(&tagsInherited.Internal.TagBitFlags, &tagsLocal.Internal.TagBitFlags, &tagsGlobal.Internal.TagBitFlags, internalTagLabel);
                ImGui::PopID();
            }

            set.GetTagsHandle().SetLocal(tagsLocal);
        }
    }

    void SetInspector::DrawCSpriteWidget(Renderer2D::CSprite& sprite, Set set)
    {
        DrawQuadWidget(sprite, true);
    }

    void SetInspector::DrawCTileWidget(Renderer2D::CTile& tile, Set set)
    {
        DrawQuadWidget(tile, false);
    }

    void SetInspector::DrawQuadWidget(Renderer2D::Quad& quad, bool transparency)
    { 
        auto& textures = TextureLibrary::GetAll();
        auto flat_color_texture = TextureLibrary::Get("Base2DTexture");
        
        auto item_current = quad.Texture;
        const char* combo_preview_value = item_current->GetID() == flat_color_texture->GetID() ? "None" : item_current->GetName().c_str();
        if (ImGui::BeginCombo("Texture", combo_preview_value))
        {
            bool is_selected = (item_current->GetID() == flat_color_texture->GetID());

            if (ImGui::Selectable("None", is_selected))
                item_current = flat_color_texture;

            for (auto it = textures.begin(); it != textures.end(); ++it)
            {
                if (!transparency && it->second->GetFormat() != TextureFormat::RGB)
                    continue;

                if (it->second->GetID() == flat_color_texture->GetID())
                    continue;

                is_selected = (item_current->GetID() == it->second.get()->GetID());
                if (ImGui::Selectable(it->first.c_str(), is_selected))
                    item_current = it->second;
                
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        
        quad.Texture = item_current;

        if (item_current->GetID() == flat_color_texture->GetID())
        {
            if (transparency)
                ImGui::ColorEdit4("Color", glm::value_ptr(quad.Color));
            else
                ImGui::ColorEdit3("Color", glm::value_ptr(quad.Color));
        }
        else
        {
            ImGui::DragFloat("Tiling Factor", &quad.TextureTilingFactor, 0.1f);
        
            if (transparency)
                ImGui::ColorEdit4("Tint Color", glm::value_ptr(quad.Color));
            else
                ImGui::ColorEdit3("Tint Color", glm::value_ptr(quad.Color));
        }
    }

    void SetInspector::DrawCCameraWidget(CCamera& camera, Set set)
    {
        bool primary = set.ID() == set.GetScene()->GetSetWithPrimaryCamera().ID();

        if (ImGui::Checkbox("Primary", &primary))
            if(primary)
                set.GetScene()->SetPrimaryCameraSet(set);

        constexpr char* projectionTypeStrings[] = { "Orthographic", "Perspective" };
        const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

        if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
        {
            for (int i = 0; i < 2; i++)
            {
                bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
                {
                    currentProjectionTypeString = projectionTypeStrings[i];
                    camera.SetProjectionType((CCamera::ProjectionType)i);
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
    
        if (camera.GetProjectionType() == CCamera::ProjectionType::Perspective)
        {
            float verticalFov = glm::degrees(camera.GetPerspectiveFOV());
            if (ImGui::DragFloat("Field of View", &verticalFov))
                camera.SetPerspectiveFOV(glm::radians(verticalFov));

            float orthoNear = camera.GetPerspectiveNearClip();
            if (ImGui::DragFloat("Near Clip", &orthoNear))
                camera.SetPerspectiveNearClip(orthoNear);

            float orthoFar = camera.GetPerspectiveFarClip();
            if (ImGui::DragFloat("Far Clip", &orthoFar))
                camera.SetPerspectiveFarClip(orthoFar);
        }
        else
        {
            float zoom = camera.GetOrthographicZoom();
            if (ImGui::DragFloat("Zoom", &zoom))
                camera.SetOrthographicZoom(zoom);

            float orthoNear = camera.GetOrthographicNearClip();
            if (ImGui::DragFloat("Near Clip", &orthoNear))
                camera.SetOrthographicNearClip(orthoNear);

            float orthoFar = camera.GetOrthographicFarClip();
            if (ImGui::DragFloat("Far Clip", &orthoFar))
                camera.SetOrthographicFarClip(orthoFar);
        }
    }

}