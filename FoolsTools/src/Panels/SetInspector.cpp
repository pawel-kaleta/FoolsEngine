#include "SetInspector.h"

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

	void SetInspector::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

		ImGui::Begin("Set Inspector");

        if (m_OpenedSetID == NullSetID)
        {
            ImGui::End();
            return;
        }

	    Set set(m_OpenedSetID, m_Scene.get());
        

        RenderCNameWidget(set);
        RenderCTransformWidget(set);
        RenderCTagsWidget(set);
        
        auto sprite = set.GetIfExist<Renderer2D::CSprite>();
        if (sprite) RenderQuadWidget(*sprite, true);
        
        auto tile = set.GetIfExist<Renderer2D::CTile>();
        if (tile) RenderQuadWidget(*tile, false);
        

		ImGui::End();
	}

    void SetInspector::RenderCNameWidget(Set set)
    {
        auto& name = set.Get<CName>();
        static char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy_s(buffer, sizeof(buffer), name.Name.c_str());
        if (ImGui::InputText("Name", buffer, sizeof(buffer)))
        {
            name.Name = std::string(buffer);
        }
    }

    void SetInspector::RenderCTransformWidget(Set set)
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_None))
        {
            auto transform = set.GetTransformHandle().Local();

            ImGui::DragFloat3("Position", glm::value_ptr(transform.Position), 0.01f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(transform.Rotation), 0.1f);
            ImGui::DragFloat3("Scale", glm::value_ptr(transform.Scale), 0.01f);

            set.GetTransformHandle().SetLocal(transform);
        }
    }

    void SetInspector::RenderTagsTable(uint32_t* tagsInherited, uint32_t* tagsLocal, uint32_t* tagsGlobal, const char* const* tagLabels)
    {    
        constexpr ImGuiTableFlags flags =
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersOuter |
            ImGuiTableFlags_BordersV |
            ImGuiTableFlags_Resizable;

        if (ImGui::BeginTable("split", 4, flags, ImVec2(0, 150)))
        {
            ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
            ImGui::TableSetupColumn("#1", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Parent", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Local", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Global", ImGuiTableColumnFlags_None);
            ImGui::TableHeadersRow();

            for (int i = 0; i < 31; i++)
            {
                ImGui::PushID(i);

                ImGui::TableNextColumn();
                ImGui::Text(tagLabels[i]);

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

                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }

    void SetInspector::RenderCTagsWidget(Set set)
    {
        constexpr char* nodeLabels[] = { "Common", "Internal" };
        constexpr char* commonTagLabel[] = {
            " 0 Error",
            " 1 Player",
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
            "30"
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
            "30"
        };

        if (ImGui::CollapsingHeader("Tags", ImGuiTreeNodeFlags_None))
        {
            auto tagsGlobal = set.GetTagsHandle().Global();
            auto tagsLocal = set.GetTagsHandle().Local();
            Tags tagsInherited;
            SetID parent = m_Scene->GetRegistry().get<CHierarchyNode>(set.ID()).Parent;
            if (parent != RootID)
                tagsInherited = m_Scene->GetHierarchy().GetComponent<CTags, Tags>(parent).Global();

            if (ImGui::TreeNodeEx((void*)nodeLabels[0], ImGuiTreeNodeFlags_OpenOnArrow, nodeLabels[0]))
            {
                RenderTagsTable(&tagsInherited.Common.TagBitFlags, &tagsLocal.Common.TagBitFlags, &tagsGlobal.Common.TagBitFlags, commonTagLabel);

                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx((void*)nodeLabels[1], ImGuiTreeNodeFlags_OpenOnArrow, nodeLabels[1]))
            {
                RenderTagsTable(&tagsInherited.Internal.TagBitFlags, &tagsLocal.Internal.TagBitFlags, &tagsGlobal.Internal.TagBitFlags, internalTagLabel);
            
                ImGui::TreePop();
            }

            set.GetTagsHandle().SetLocal(tagsLocal);
        }
    }

    void SetInspector::RenderQuadWidget(Renderer2D::Quad& quad, bool transparency)
    {
        if (transparency)
        {
            if (!ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_None))
                return;
        }
        else
        {
            if (!ImGui::CollapsingHeader("Tile", ImGuiTreeNodeFlags_None))
                return;
        }
        
        static int e = (quad.Texture.get()->GetID() != TextureLibrary::Get("Base2DTexture").get()->GetID());
        ImGui::RadioButton("Flat Color", &e, 0); ImGui::SameLine();
        ImGui::RadioButton("Texture", &e, 1);
        
        if (e == 0)
        {
            if (quad.Texture.get()->GetID() != TextureLibrary::Get("Base2DTexture").get()->GetID())
                quad.Texture = TextureLibrary::Get("Base2DTexture");
            
            if (transparency)
                ImGui::ColorEdit4("Color", glm::value_ptr(quad.Color));
            else
                ImGui::ColorEdit3("Color", glm::value_ptr(quad.Color));
        }
        if (e == 1)
        {
            ImGui::PushID("1"); //ID conflict RadioButton("Texture") and BeginCombo("Texture")
            
            auto& textures = TextureLibrary::GetAll();
            
            auto item_current = quad.Texture;
            const char* combo_preview_value = item_current->GetName().c_str();
            if (ImGui::BeginCombo("Texture", combo_preview_value, 0))
            {
                for (auto it = textures.begin(); it != textures.end(); ++it)
                {
                    const bool is_selected = (item_current->GetID() == it->second.get()->GetID());
                    if (ImGui::Selectable(it->first.c_str(), is_selected))
                        item_current = it->second;
                    
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            
            quad.Texture = item_current;
            
            ImGui::DragFloat("Tiling Factor", &quad.TextureTilingFactor, 0.1f);
            
            if (transparency)
                ImGui::ColorEdit4("Tint Color", glm::value_ptr(quad.Color));
            else
                ImGui::ColorEdit3("Tint Color", glm::value_ptr(quad.Color));
            
            ImGui::PopID();
        }
    }


}