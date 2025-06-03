#include "TextureImport.h"

#include "ImportData.h"

#include <filesystem>

namespace fe
{
	void TextureImport::InitImport(ImportData* const importData)
	{
        importData->TextureData.Specification = TextureLoader::InspectTexture(importData->Filepath);
	}

    static void Import(const ImportData* const importData)
    {
        FE_CORE_ASSERTION_BREAK(false, "Not implemented");
        AssetID assetID;// = AssetManager::CreateAsset<Texture2D>();
        AssetHandle<Texture2D> textureHandle(assetID);
        auto texture_user = textureHandle.Use();

        texture_user.GetCoreComponent().Specification = importData->TextureData.Specification;
        //texture_user.SetFilepath(importData->Filepath);

        if (importData->HandleToOverride && importData->Type == AssetType::Texture2DAsset)
            *(AssetHandle<Texture2D>*)(importData->HandleToOverride) = textureHandle;
    }

    void TextureImport::RenderWindow(ImportData* const importData)
    {
        auto& spec = importData->TextureData.Specification;

        ImGui::Text("File: %s", importData->Filepath.string().c_str());
        ImGui::Text("Components: %d", spec.Components);
        ImGui::Text("Format: %d", spec.Format);
        ImGui::Text("Width: %d", spec.Width);
        ImGui::Text("Height: %d", spec.Height);
        ImGui::Separator();
        ImGui::Text("TO DO: import settings");

        ImGui::Separator();
        if (ImGui::Button("Import"))
        {
            Import(importData);

            importData->Finished = true;
        }
    }
}