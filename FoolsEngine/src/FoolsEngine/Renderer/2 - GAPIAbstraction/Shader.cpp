#include "FE_pch.h"
#include "Shader.h"

#include "OpenGL\OpenGLShader.h"

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\1 - Description\Uniform.h"
#include "FoolsEngine\Renderer\1 - Description\ShaderTextureSlot.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include "FoolsEngine/Assets/Loaders/ShaderLoader.h"

namespace fe
{
    void ACShaderCore::Init()
    {
        VertexSource.clear();
        FragmentSource.clear();
        ShaderSource.clear();
    }

    void ShaderObserver::Bind(GAPIType GAPI) const
    {
        switch (GAPI.Value)
        {
        case GAPIType::None:
            FE_CORE_ASSERT(false, "Unspecified GAPIType");
            return;

        case GAPIType::OpenGL:
            Get<OpenGLShader>().Bind();
            return;
        }
    }

    void ShaderObserver::Unbind(GAPIType GAPI) const
    {
        switch (GAPI.Value)
        {
        case GAPIType::None:
            FE_CORE_ASSERT(false, "Unspecified GAPIType");
            return;

        case GAPIType::OpenGL:
            Get<OpenGLShader>().Unbind();
            return;
        }
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
            if (AllOf<OpenGLShader>())
                Erase<OpenGLShader>();
            return;
        }
    }

    void ShaderUser::SendDataToGPU(GAPIType GAPI, void* data)
    {
        FE_CORE_ASSERT(false, "Shader loading not implemented yet");
    }

    void ShaderUser::UnloadFromCPU() const
    {
        auto& sourceCode = Get<ACShaderCore>();
        sourceCode.ShaderSource.clear();
        sourceCode.VertexSource.clear();
        sourceCode.FragmentSource.clear();
    }

    uint32_t ShaderObserver::GetRendererID(GAPIType GAPI) const
    {
        switch (GAPI.Value)
        {
        case GAPIType::None:
            FE_CORE_ASSERT(false, "Unspecified GAPIType");
            return 0;

        case GAPIType::OpenGL:
            return Get<OpenGLShader>().GetProgramID();
            break;
        }
        return 0;
    }

    void ShaderObserver::UploadUniform(GAPIType GAPI, const Uniform& uniform, const void* dataPointer, uint32_t count, bool transpose) const
    {
        switch (GAPI.Value)
        {
        case GAPIType::None:
            FE_CORE_ASSERT(false, "Unspecified GAPIType");
            return;

        case GAPIType::OpenGL:
            Get<OpenGLShader>().UploadUniform(uniform, dataPointer, count, transpose);
            return;
        }
    }

    void ShaderObserver::BindTextureSlot(GAPIType GAPI, const ShaderTextureSlot& textureSlot, RenderTextureSlotID* rendererTextureSlot, uint32_t count) const
    {
        switch (GAPI.Value)
        {
        case GAPIType::None:
            FE_CORE_ASSERT(false, "Unspecified GAPIType");
            return;

        case GAPIType::OpenGL:
            Get<OpenGLShader>().BindTextureSlot(textureSlot, rendererTextureSlot, count);
            return;
        }
    }

    void ShaderObserver::BindTextureSlot(GAPIType GAPI, const ShaderTextureSlot& textureSlot, RenderTextureSlotID rendererTextureSlot) const
    {
        switch (GAPI.Value)
        {
        case GAPIType::None:
            FE_CORE_ASSERT(false, "Unspecified GAPIType");
            return;

        case GAPIType::OpenGL:
            Get<OpenGLShader>().BindTextureSlot(textureSlot, rendererTextureSlot);
            return;
        }
    }

}