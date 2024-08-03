#include "TextureImport.h"

#include <FoolsEngine.h>

namespace fe
{
    std::filesystem::path TextureImport::s_Filepath;
    TextureData::Specification TextureImport::s_Specification;
    static char str0[128];

	void TextureImport::OpenWindow(std::filesystem::path filepath)
	{
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        s_Filepath = filepath;
        s_Specification = TextureLoader::InspectTexture(s_Filepath);

        memset(str0, 0, sizeof(str0));
        strncpy_s(str0, "Texture name", sizeof(str0));

        ImGui::OpenPopup("Texture Import");
	}

    void TextureImport::RenderWindow()
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Texture Import", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("File: %s", s_Filepath.string().c_str());
            ImGui::Text("Components: %d", s_Specification.Components);
            ImGui::Text("Format: %d", s_Specification.Format);
            ImGui::Text("Width: %d", s_Specification.Width);
            ImGui::Text("Height: %d", s_Specification.Height);
            ImGui::Separator();
            ImGui::Text("TO DO: import settings");
            ImGui::Separator();

			if (ImGui::Button("Import As..."))
			{
                std::filesystem::path defaultFilepath = s_Filepath;
                defaultFilepath.replace_extension(std::filesystem::path(".fetex2d"));
				const std::filesystem::path newTextureFilepath = FileDialogs::SaveFile(defaultFilepath.string().c_str(), "Texture2D (*.fetex2d)\0 *.fetex2d\0");
				if (!newTextureFilepath.empty())
				{
                    auto textureId = AssetManager::CreateAsset<Texture2D>(newTextureFilepath);
                    auto textureHandle = AssetHandle<Texture2D>(textureId);

                    YAML::Emitter emitter;

                    emitter << YAML::BeginMap;
                    emitter << YAML::Key << "UUID"          << YAML::Value << textureHandle.GetUUID();
                    emitter << YAML::Key << "Source File"   << YAML::Value << s_Filepath.string();
                    emitter << YAML::Key << "Components"    << YAML::Value << (uint32_t)s_Specification.Components;
                    emitter << YAML::Key << "Format"        << YAML::Value << (uint32_t)s_Specification.Format;
                    emitter << YAML::Key << "Width"         << YAML::Value << s_Specification.Width;
                    emitter << YAML::Key << "Height"        << YAML::Value << s_Specification.Height;
                    emitter << YAML::EndMap;

                    std::ofstream fout(newTextureFilepath);
                    fout << emitter.c_str();

                    ImGui::CloseCurrentPopup();
				}
			}
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }
    }
}