#include "TextureImport.h"

#include "ImportData.h"

#include <filesystem>

namespace fe
{
	void TextureImport::InitImport(ImportData*  importData)
	{
        if (importData->Type != AssetType::None)
        {
            FE_CORE_ASSERT(importData->Type == AssetType::Texture2D, "Attempt to init texture import with wrong asset type set in importData");
            FE_CORE_ASSERT(importData->HandleToOverride, "Attempt to init texture import for asset handle with asset handle pointer being null");
        }
        else
        {
            FE_CORE_ASSERT(!importData->HandleToOverride, "Attempt to init texture import with asset type set to None in importData");
        }
        
        importData->TextureData.Specification = TextureLoader::InspectTexture(importData->Filepath);
	}

    static void Import(const ImportData* importData)
    {
        Scratchpad sp;
        auto& source_filepath = importData->Filepath;

        auto meta_filepath = source_filepath.parent_path() / importData->AssetName;
        meta_filepath += std::pmr::string(Texture2D::GetMetaFileExtension(), &sp);
        AssetID assetID = AssetManager::AssetCreation::ProjectAsset<Texture2D>(meta_filepath);
        AssetHandle<Texture2D> textureHandle(assetID);
        textureHandle.Use().GetCoreComponent().Specification = importData->TextureData.Specification;
        AssetManager::SetSourcePath(assetID, source_filepath);

        Texture2D::SaveMetadata(assetID);

        if (importData->HandleToOverride && importData->Type == AssetType::Texture2D)
            *(AssetHandle<Texture2D>*)(importData->HandleToOverride) = textureHandle;
    }

    void TextureImport::RenderWindow(ImportData* const importData)
    {
        auto& spec = importData->TextureData.Specification;
        Scratchpad sp;
        ImGui::Text("File: %s", importData->Filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str());
        ImGui::Text("Components: %d", spec.Components);
        ImGui::Text("Format: %d", spec.Format);
        ImGui::Text("Width: %d", spec.Width);
        ImGui::Text("Height: %d", spec.Height);

        if (importData->ImportedAssets)
        {
            ImGui::SeparatorText("Imported Assets from this Source");

            Scratchpad sp2;
            for (auto& assetID : *importData->ImportedAssets)
            {
                AssetObserver<Texture2D> assetObserver(assetID);
                const auto asset_name = assetObserver.GetFilepath().stem().string<PMR_STRING_TEMPLATE_PARAMS>(&sp2);
                if (ImGui::TreeNodeEx(asset_name.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth, asset_name.c_str()))
                {
                    ImGui::Text("Usage: ");
                    ImGui::SameLine();
                    ImGui::Text(assetObserver.GetCoreComponent().Specification.Usage.ToConstCharPtr());
                    ImGui::TreePop();
                }
            }
        }
        ImGui::SeparatorText("Settings");

        ImGui::InputText("Asset Name", importData->AssetName, sizeof(importData->AssetName));

#define _COMBO_ITEM_DEF(x) #x,
        const char* usage_names[] = { FE_FOR_EACH(_COMBO_ITEM_DEF, FE_TEXTURE_DATA_USAGE) };
        static int usage_current = 0;
        ImGui::Combo("Usage", &usage_current, usage_names, IM_ARRAYSIZE(usage_names));

        ImGui::Separator();

        bool name_conflict = false;
        auto potencial_meta_filepath = importData->Filepath.parent_path() / importData->AssetName;
        potencial_meta_filepath += std::pmr::string(Texture2D::GetMetaFileExtension(), &sp);
        if (AssetManager::GetAssetFromFilepath(potencial_meta_filepath) != NullAssetID)
        {
            ImGui::BeginDisabled();
            name_conflict = true;
        }
        else if (importData->ImportedAssets)
        {
            Scratchpad sp2;
            for (auto& assetID : *importData->ImportedAssets)
            {
                const auto asset_name = AssetObserver<Texture2D>(assetID).GetFilepath().stem().string<PMR_STRING_TEMPLATE_PARAMS>(&sp2);
                if (importData->AssetName == asset_name)
                {
                    ImGui::BeginDisabled();
                    name_conflict = true;
                    break;
                }
            }
        }

        if (ImGui::Button("Import"))
        {
            spec.Usage.FromInt(usage_current);
            Import(importData);

            importData->Finished = true;
        }

        if (name_conflict)
        {
            ImGui::SetItemTooltip("Name occupied");
            ImGui::EndDisabled();
        }
    }
}