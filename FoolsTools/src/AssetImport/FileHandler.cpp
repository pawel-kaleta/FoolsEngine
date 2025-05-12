#include "FileHandler.h"

#include "ImportData.h"
#include "TextureImport.h"
#include "GeometryImport.h"

#include <FoolsEngine.h>

namespace fe
{
    constexpr static size_t LoadersCount = 4;

    namespace FileHandler
    {
        struct LoaderData
        {
            bool (* const IsKnownExtensionFunkPtr)(const std::pmr::string&);
            const char* SourceExtensionAlias;
            AssetType AssetType;
        };
        
        const static LoaderData s_LoaderData[LoadersCount] =
        {
            { nullptr,                            "",                                  AssetType::SceneAsset },
            { & TextureLoader::IsKnownExtension,  TextureLoader::GetExtensionAlias(),  AssetType::Texture2DAsset },
            { & GeometryLoader::IsKnownExtension, GeometryLoader::GetExtensionAlias(), AssetType::None }, // multiple asset types
            { & ShaderLoader::IsKnownExtension,   ShaderLoader::GetExtensionAlias(),   AssetType::ShaderAsset }
        };
       
        uint32_t GetSourceAliasAndLoaderIndex(const std::pmr::string& extension, std::pmr::string& outAlias)
        {
            for (size_t i = 0; i < LoadersCount; i++)
            {
                auto& loader_data = s_LoaderData[i];
                if (!loader_data.IsKnownExtensionFunkPtr)
                    continue;

                if ((*loader_data.IsKnownExtensionFunkPtr)(extension))
                {
                    outAlias = loader_data.SourceExtensionAlias;
                    return i;
                }
            }

            return -1;
        }

	    void OpenFile(const std::filesystem::path& filepath)
	    {
		    if (filepath.empty())
		    {
			    FE_CORE_ASSERT(false, "Attempt to open file without filepath");
			    return;
		    }

		    for (size_t i = 0; i < LoadersCount; i++)
		    {
                auto& loader_data = s_LoaderData[i];

                if (AssetManager::GetAssetFromFilepath(filepath, loader_data.AssetType))
                {
                    //asset allready imported
                    return;
                }

			    if (!loader_data.IsKnownExtensionFunkPtr)
				    continue;

			    if ((*loader_data.IsKnownExtensionFunkPtr)(filepath.extension().string<PMR_STRING_TEMPLATE_PARAMS>()))
			    {
				    AssetImportModal::OpenWindow(filepath, i, AssetType::None, nullptr);
				    return;
			    }
		    }
	    }
    }

    namespace AssetImportModal
    {
        ImportData* s_ImportData = nullptr;

        struct ImporterData
        {
            void (* const InitImportFunkPtr)(ImportData* const);
            void (* const RenderWindowFunkPtr)(ImportData* const);
        };

        const static ImporterData s_ImporterData[LoadersCount] =
        {
            { nullptr, nullptr },
            { & TextureImport::InitImport,  & TextureImport::RenderWindow },
            { & GeometryImport::InitImport, & GeometryImport::RenderWindow },
            { nullptr, nullptr }
        };

        void OnImGuiRender()
        {
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (s_ImportData)
                ImGui::OpenPopup("Asset Import");
            if (ImGui::BeginPopupModal("Asset Import", NULL, 0))
            {
                FE_CORE_ASSERT(s_ImportData, "");
                auto& render_window_funk = s_ImporterData[s_ImportData->LoaderIndex].RenderWindowFunkPtr;
                (*render_window_funk)(s_ImportData);

                if (s_ImportData->Finished)
                {
                    delete s_ImportData;
                    s_ImportData = nullptr;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                {
                    delete s_ImportData;
                    s_ImportData = nullptr;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        void OpenWindow(const std::filesystem::path& filepath, uint32_t loaderIndex, AssetType type, AssetHandleBase* optionalBaseHandle)
        {
            s_ImportData = new ImportData;

            s_ImportData->Filepath = filepath;
            s_ImportData->Type = type;
            s_ImportData->HandleToOverride = optionalBaseHandle;
            s_ImportData->LoaderIndex = loaderIndex;

            auto& init_import_funk = s_ImporterData[loaderIndex].InitImportFunkPtr;
            (*init_import_funk)(s_ImportData);
        }
    }
}