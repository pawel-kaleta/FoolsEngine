#include "AssetImportModal.h"
#include "TextureImport.h"

#include <FoolsEngine.h>

#include <filesystem>

namespace fe::AssetImportModal
{
    std::filesystem::path s_Filepath;
    AssetType s_Type;
    bool s_Opened = false;

    void (*s_InitImportFunkPtrs[AssetType::Count])(const std::filesystem::path&, AssetHandleBase*)
    {
        nullptr, //SceneAsset
        nullptr, //TextureAsset
        &TextureImport::InitImport,
        nullptr, //MeshAsset
        nullptr, //ModelAsset
        nullptr, //ShaderAsset
        nullptr, //MaterialAsset
        nullptr, //MaterialInstanceAsset
        nullptr  //AudioAsset
    };

    void (*s_RenderWindowFunkPtrs[AssetType::Count])()
    {
        nullptr, //SceneAsset
        nullptr, //TextureAsset
        &TextureImport::RenderWindow,
        nullptr, //MeshAsset
        nullptr, //ModelAsset
        nullptr, //ShaderAsset
        nullptr, //MaterialAsset
        nullptr, //MaterialInstanceAsset
        nullptr  //AudioAsset
    };

    void (*s_ImportAs[AssetType::Count])(const std::filesystem::path&)
    {
        nullptr, //SceneAsset
        nullptr, //TextureAsset
        &TextureImport::ImportAs,
        nullptr, //MeshAsset
        nullptr, //ModelAsset
        nullptr, //ShaderAsset
        nullptr, //MaterialAsset
        nullptr, //MaterialInstanceAsset
        nullptr  //AudioAsset
    };

	void OnImGuiRender()
	{
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (s_Opened)
            ImGui::OpenPopup("Asset Import");
        if (ImGui::BeginPopupModal("Asset Import", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            (*s_RenderWindowFunkPtrs[s_Type])();

            ImGui::Separator();
            if (ImGui::Button("Import As..."))
            {
                std::filesystem::path defaultFilepath = s_Filepath;
                defaultFilepath.replace_extension(std::filesystem::path(Texture2D::GetProxyExtension()));
                const std::filesystem::path newTextureFilepath = FileDialogs::SaveFile(defaultFilepath.string().c_str(), "Texture2D (*.fetex2d)\0 *.fetex2d\0");

                (*s_ImportAs[s_Type])(newTextureFilepath);

                s_Opened = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                s_Opened = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
	}

    void OpenWindow(const std::filesystem::path& filepath, AssetType type, AssetHandleBase* optionalBaseHandle)
    {
        s_Filepath = filepath;
        s_Type = type;

        (*s_InitImportFunkPtrs[type])(filepath, optionalBaseHandle);

        s_Opened = true;
    }
}