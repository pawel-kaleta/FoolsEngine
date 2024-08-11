#include "TextureImport.h"

#include <FoolsEngine.h>

#include <filesystem>

namespace fe
{
    std::filesystem::path TextureImport::s_Filepath;
    TextureData::Specification TextureImport::s_Specification;
    AssetHandle<Texture2D>* s_Handle;
    static char str0[128];

	void TextureImport::InitImport(const std::filesystem::path& filepath, AssetHandleBase* optionalBaseHandle)
	{
        s_Handle = (AssetHandle<Texture2D>*)optionalBaseHandle;
        s_Filepath = filepath;
        s_Specification = TextureLoader::InspectTexture(filepath);

        memset(str0, 0, sizeof(str0));
        strncpy_s(str0, "Texture name", sizeof(str0));
	}

    void TextureImport::RenderWindow()
    {
        ImGui::Text("File: %s", s_Filepath.string().c_str());
        ImGui::Text("Components: %d", s_Specification.Components);
        ImGui::Text("Format: %d", s_Specification.Format);
        ImGui::Text("Width: %d", s_Specification.Width);
        ImGui::Text("Height: %d", s_Specification.Height);
        ImGui::Separator();
        ImGui::Text("TO DO: import settings");
    }

    void TextureImport::ImportAs(const std::filesystem::path& filepath)
    {
        if (filepath.empty())
            return;

        YAML::Emitter emitter;

        emitter << YAML::BeginMap;
        emitter << YAML::Key << "UUID" << YAML::Value << UUID();
        emitter << YAML::Key << "Source File" << YAML::Value << s_Filepath.string();
        emitter << YAML::Key << "Components" << YAML::Value << (uint32_t)s_Specification.Components;
        emitter << YAML::Key << "Format" << YAML::Value << (uint32_t)s_Specification.Format;
        emitter << YAML::Key << "Width" << YAML::Value << s_Specification.Width;
        emitter << YAML::Key << "Height" << YAML::Value << s_Specification.Height;
        emitter << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << emitter.c_str();
        fout.close();

        AssetID assetID = AssetManager::CreateAsset<Texture2D>(filepath);
        AssetHandle<Texture2D> textureHandle(assetID);

        if (s_Handle)
            *s_Handle = textureHandle;
        s_Handle = nullptr;

        textureHandle.Use().GetOrEmplaceSpecification().Specification = s_Specification;
    }
}