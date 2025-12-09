#include "FE_pch.h"
#include "ShaderAsset.h"

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include "FoolsEngine\Assets\Serialization\YAML.h"

namespace fe
{
    void ACShaderAssetCore::Init()
    {
        SpecificationID = -1;
    }

    void ShaderAssetObserver::SaveMetadata(YAML::Emitter& emitter)
    {
        const auto& core = GetCoreComponent();
        const auto& library = Description::Library::Get();
        const auto& spec = library.ShaderSpecs[core.SpecificationID];

        /*ShaderType Type;

			uint32_t InputLayoutID;
			uint32_t OutputLayoutID;
			uint32_t MainUniformsLayoutID;

			std::pmr::vector<uint32_t> TextureSamplerIDs;
			std::pmr::vector<uint32_t> UniformBufferSamplerIDs;
			std::pmr::vector<uint32_t> DynamicBufferSamplerIDs;*/
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "UUID" << YAML::Value << GetUUID();
        if (!AllOf<ACMasterAsset>())
            emitter << YAML::Key << "Source Filepath" << YAML::Value << GetSourceFilepath()->Filepath.string();
        emitter << YAML::Key << "Type" << YAML::Value << spec.Type.ToConstCharPtr();
        emitter << YAML::Key << "Index Count" << YAML::Value << core.Specification.IndexCount;
        emitter << YAML::EndMap;
    }

    void ShaderAssetUser::Release() const
    {
        auto GAPI = Renderer::GetActiveGAPIType();
        switch (GAPI.Value)
        {
        case GAPIType::None:
            FE_CORE_ASSERT(false, "Unspecified GAPIType");
            return;

        case GAPIType::OpenGL:
            if (AllOf<ACShaderResource_OpenGL>())
            {
                Get<ACShaderResource_OpenGL>().Shader.Destroy();
                Erase<ACShaderResource_OpenGL>();
            }
            return;
        }
    }

    void ShaderAssetUser::SendDataToGPU(GAPIType GAPI, void* data)
    {
        FE_CORE_ASSERT(false, "Shader loading not implemented yet");

        Scratchpad sp;

        std::ifstream in(Get<ACSourceFilepath>().Filepath, std::ios::in, std::ios::binary);

        if (!in.good())
        {
            FE_LOG_CORE_ERROR("Could not load shader file: \"{0}\"", Get<ACSourceFilepath>().Filepath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp));
            return;
        }

        std::pmr::string shader_source(&sp);

        in.seekg(0, std::ios::end);
        shader_source.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&shader_source[0], shader_source.size());
        in.close();

        switch (GAPI.Value)
        {
        case GAPIType::None:
            FE_CORE_ASSERT(false, "Unspecified GAPIType");
            return;

        case GAPIType::OpenGL:
            if (AllOf<ACShaderResource_OpenGL>())
            {
                auto& shader = Emplace<ACShaderResource_OpenGL>().Shader;

                shader.SpecificationID = GetCoreComponent().SpecificationID;
                shader.Create(shader_source.c_str());
            }
            return;
        }
    }

    void ShaderAssetUser::UnloadFromCPU() const
    {
        //auto& sourceCode = Get<ACShaderAssetCore>();
        //sourceCode.ShaderSource.clear();
    }


}