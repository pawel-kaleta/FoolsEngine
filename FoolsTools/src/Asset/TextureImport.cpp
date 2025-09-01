#include "TextureImport.h"

#include "ImportData.h"
#include "FileHandler.h"

#include <filesystem>

namespace fe
{
	void TextureImport::InitImport(ImportData*  importData)
	{      
        importData->TextureData.Specification = TextureLoader::InspectTexture(importData->FilepathToImport);
	}

    static void Import(const std::filesystem::path& filepath, const ImportData* importData)
    {
        auto assets_path = Project::GetInstance()->AssetsPath;
        auto x = filepath.lexically_relative(std::filesystem::current_path());
        auto w = x.lexically_relative(assets_path);

        AssetID assetID = AssetManager::AssetCreation::ProjectAsset<Texture2D>(w);
        AssetHandle<Texture2D> textureHandle(assetID);
        textureHandle.Use().GetCoreComponent().Specification = importData->TextureData.Specification;
        AssetManager::SetSourcePath(assetID, importData->FilepathToImport.lexically_relative(assets_path));

        YAML::Emitter emitter;
        Texture2D::SaveMetadata(emitter, assetID);
        std::ofstream fout(Project::GetInstance()->AssetsPath / AssetObserver<Texture2D>(assetID).GetFilepath());
        fout << emitter.c_str();
    }

    void TextureImport::RenderWindow(ImportData* const importData)
    {
        auto& spec = importData->TextureData.Specification;
        Scratchpad sp;
        ImGui::Text("File: %s", importData->FilepathToImport.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str());
        ImGui::Text("Components: %d", spec.Components);
        ImGui::Text("Format: %d", spec.Format);
        ImGui::Text("Width: %d", spec.Width);
        ImGui::Text("Height: %d", spec.Height);

        ImGui::SeparatorText("Settings");

#define _COMBO_ITEM_DEF(x) #x,
        static const char* usage_names[] = { FE_FOR_EACH(_COMBO_ITEM_DEF, None, FE_TEXTURE_DATA_USAGE) };
        static int usage_current = 0;
        ImGui::Combo("Usage", &usage_current, usage_names, IM_ARRAYSIZE(usage_names));

        ImGui::Separator();

        if (ImGui::Button("Import Ass..."))
        {
            spec.Usage.FromInt(usage_current);

            std::pmr::string filter(&sp);
            std::filesystem::path defaultFilepath = AssetImportModal::GetDefaultFilepathAndFilterForImport<Texture2D>(importData->FilepathToImport, filter);

            std::filesystem::path newAssetFilepath = FileDialogs::SaveFile(defaultFilepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str(), filter.c_str());

            if (!newAssetFilepath.empty())
            {
                Import(newAssetFilepath, importData);

                importData->Finished = true;
            }
        }
    }
}