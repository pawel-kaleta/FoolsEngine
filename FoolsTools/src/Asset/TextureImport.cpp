#include "TextureImport.h"

#include "ImportData.h"
#include "FileHandler.h"

#include <filesystem>

namespace fe
{
	void TextureImport::InitImport(ImportData*  importData)
	{      
        importData->Description.Specification = TextureLoader::InspectTexture(importData->FilepathToImport);
	}

    static void Import(const std::filesystem::path& filepath, const ImportData* importData)
    {
        //TO DO: clean up this path operations
        auto assets_path = Project::Get()->m_AssetsPath;
        auto x = filepath.lexically_relative(std::filesystem::current_path());
        auto w = x.lexically_relative(assets_path);

        AssetID assetID = AssetManager::AssetCreation::ProjectAsset<Texture2D>(w);
        AssetUser<Texture2D>(assetID).GetCoreComponent().Specification = importData->Description.Specification;
        AssetManager::SetSourcePath(assetID, importData->FilepathToImport.lexically_relative(assets_path));

        YAML::Emitter emitter;
        Texture2D::SaveMetadata(emitter, assetID);
        std::ofstream fout(Project::Get()->m_AssetsPath / AssetObserver<Texture2D>(assetID).GetFilepath());
        fout << emitter.c_str();

        AssetSerializer::SerializeRegistry();
    }

    void TextureImport::RenderWindow(ImportData* const importData)
    {
        auto& spec = importData->Description.Specification;
        Scratchpad sp;
        ImGui::Text("m_File: %s", importData->FilepathToImport.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str());
        //ImGui::Text("Components: %d", spec.Components);
        ImGui::Text("Format: %d", spec.Format);
        //ImGui::Text("Width: %d", spec.Width);
        //ImGui::Text("Height: %d", spec.Height);

        ImGui::SeparatorText("Settings");

#define _COMBO_ITEM_DEF(x) #x,
        static const char* s_usage_names[] = { FE_FOR_EACH(_COMBO_ITEM_DEF, None, FE_TEXTURE_DATA_USAGE) };
        static int usage_current = 0;
        ImGui::Combo("Usage", &usage_current, s_usage_names, IM_ARRAYSIZE(s_usage_names));

        ImGui::Separator();

        if (ImGui::Button("Import Ass..."))
        {
            //spec.Usage.FromInt(usage_current);

            std::pmr::string filter(&sp);
            std::filesystem::path default_filepath = AssetImportModal::GetDefaultFilepathAndFilterForImport<Texture2D>(importData->FilepathToImport, filter);

            std::filesystem::path new_asset_filepath = FileDialogs::SaveFile(default_filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp).c_str(), filter.c_str());

            if (!new_asset_filepath.empty())
            {
                Import(new_asset_filepath, importData);

                importData->Finished = true;
            }
        }
    }
}