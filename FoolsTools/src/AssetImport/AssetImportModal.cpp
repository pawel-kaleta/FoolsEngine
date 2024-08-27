#include "AssetImportModal.h"
#include "TextureImport.h"
#include "MeshImport.h"

#include <FoolsEngine.h>

#include <filesystem>

namespace fe
{
    extern std::string(*GetProxyExtensionPtrs[AssetType::Count])();
}

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
        &MeshImport::InitImport,
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
        &MeshImport::RenderWindow,
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
        &MeshImport::ImportAs,
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
                std::string extension = (*GetProxyExtensionPtrs[s_Type])();
                defaultFilepath.replace_extension(std::filesystem::path(extension));

                std::string filter = "NewAsset (*" + extension + ")\0 *" + extension + "\0";
                const std::filesystem::path newTextureFilepath = FileDialogs::SaveFile(defaultFilepath.string().c_str(), filter.c_str());

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