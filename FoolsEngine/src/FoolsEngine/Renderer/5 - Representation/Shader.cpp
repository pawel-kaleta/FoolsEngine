#include "FE_pch.h"
#include "Shader.h"

#include "FoolsEngine/Assets/Serialization/YAML.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/7 - Integration/Renderer.h"

namespace fe
{
    void ACShaderCore::Init()
    {
        SpecificationID = -1;
    }

    void ShaderObserver::SaveMetadata(YAML::Emitter& emitter)
    {
        const auto& core = GetCore();
        const auto& library = Description::Library::Get();
        const auto& spec = library.ShaderSpecs[core.SpecificationID];

        emitter << YAML::BeginMap;
        emitter << YAML::Key << "UUID" << YAML::Value << GetUUID();
        if (!AllOf<ACMasterAsset>())
            emitter << YAML::Key << "Source Filepath" << YAML::Value << GetSourceFilepath()->Filepath.string();
        emitter << YAML::Key << "Type" << YAML::Value << spec.Type.ToConstCharPtr();
        emitter << YAML::Key << "Specification" << YAML::Value << spec.UUID;
        emitter << YAML::EndMap;
    }

    bool ShaderUser::LoadMetadata()
    {
        const auto& filepath = GetFilepath();
        auto& reg = AssetManager::Get().m_Registry;

        YAML::Node node;

        {
            FE_PROFILER_SCOPE("YAML::LoadFile");
            node = YAML::LoadFile(filepath.string());
        }

        auto uuid_node = node["UUID"];
        if (uuid_node) // Base Assets don't have UUID in their file
        {
            if (reg.get<ACUUID>(GetID()).UUID != node["UUID"].as<UUID>())
            {
                FE_CORE_ASSERT(false, "Not machting UUID in asset and its serialized node");
                return false;
            }
        }
        else
        {
            FE_LOG_CORE_WARN("Missing UUID in Shader serialized node");
        }

        const auto& source_filepath_node = node["Source Filepath"];
        if (!source_filepath_node) return false;
        AssetManager::SetSourcePath(GetID(), source_filepath_node.as<std::string>());

        const auto& type = node["Type"];
        const auto& specID = node["Specification"];

        if (!type || !specID)
            return false;

        auto& core = GetCore();
        auto& lib = Description::Library::Get();
        auto spec_uuid = specID.as<UUID>();
        core.SpecificationID = lib.CreateOrGetDescriptorWithUUID<Description::ShaderInterface::Specification>(spec_uuid);

        return true;
    }

    void ShaderUser::Release() const
    {
        auto GAPI = Renderer::GetActiveGAPIType();
        switch (GAPI.Value)
        {
        case GAPIType::None:
            FE_CORE_ASSERT(false, "Unspecified GAPIType");
            return;

        case GAPIType::OpenGL:
            FE_CORE_ASSERT(false, "Not implemented");
            //if (AllOf<ACShaderResource_OpenGL>())
            //{
            //    Get<ACShaderResource_OpenGL>().Shader.Destroy();
            //    Erase<ACShaderResource_OpenGL>();
            //}
            return;
        }
    }

    void ShaderUser::UnloadFromCPU() const
    {
        FE_CORE_ASSERT(false, "Not implemented");
        auto& core = Get<ACShaderCore>();
        //core.ShaderSource.clear();
        //core.ShaderSource.shrink_to_fit();
    }
}