#include "FileHandler.h"

#include "ImportData.h"
#include "TextureImport.h"
#include "GeometryImport.h"

namespace fe
{
    namespace FileHandler
    {     
	    void OpenFile(const std::filesystem::path& filepath, const LoadersRegistry::Item* loaderItemPtr)
	    {
		    if (filepath.empty())
		    {
			    FE_CORE_ASSERT(false, "Attempt to open file without filepath");
			    return;
		    }

            if (!loaderItemPtr)
            {
                Scratchpad sp;
                const auto item = LoadersRegistry::GetItem(filepath.extension().string<PMR_STRING_TEMPLATE_PARAMS>(&sp));
				AssetImportModal::OpenWindow(filepath, item);
				return;
            }
            else
            {
                AssetImportModal::OpenWindow(filepath, loaderItemPtr);
            }
	    }
    }

    namespace AssetImportModal
    {
        ImportData* s_ImportData = nullptr;

        struct ImporterData
        {
            void (* InitImportFunkPtr)(ImportData* const);
            void (* RenderWindowFunkPtr)(ImportData* const);
        };

        static ImporterData s_ImporterData[LoaderType::Count];

        void Init()
        {
            for (auto& importerData : s_ImporterData) importerData = { nullptr, nullptr };

            s_ImporterData[LoaderType::Texture  ] = { &  TextureImport::InitImport,  &  TextureImport::RenderWindow };
            s_ImporterData[LoaderType::Geometry ] = { & GeometryImport::InitImport, & GeometryImport::RenderWindow };
        }

        void OnImGuiRender()
        {
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (s_ImportData)
                ImGui::OpenPopup("Asset Import");
            if (ImGui::BeginPopupModal("Asset Import", NULL, 0))
            {
                FE_CORE_ASSERT(s_ImportData, "");
                auto& render_window_funk = s_ImporterData[s_ImportData->LoaderItemPtr->Type.ToInt()].RenderWindowFunkPtr;
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

        void OpenWindow(const std::filesystem::path& filepathToImport, const LoadersRegistry::Item* loaderItemPtr)
        {
            if (s_ImportData)
            {
                FE_LOG_CORE_WARN("Should s_ImportData be alive before the import setup?");
                delete s_ImportData;
            }

            auto& init_import_funk = s_ImporterData[loaderItemPtr->Type.ToInt()].InitImportFunkPtr;

            if (!init_import_funk)
            {
                FE_CORE_ASSERT(false, "Import of this asset type not implemented");
                return;
            }

            s_ImportData = new ImportData();

            s_ImportData->FilepathToImport = filepathToImport;
            s_ImportData->LoaderItemPtr = loaderItemPtr;

            (*init_import_funk)(s_ImportData);
        }
    }
}