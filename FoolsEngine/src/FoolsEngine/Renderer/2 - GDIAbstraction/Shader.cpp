#include "FE_pch.h"
#include "Shader.h"

#include "OpenGL\OpenGLShader.h"

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"
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

    void ShaderObserver::Bind(GDIType GDI) const
    {
        switch (GDI.Value)
        {
        case GDIType::None:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return;

        case GDIType::OpenGL:
            Get<OpenGLShader>().Bind();
            return;
        }
    }

    void ShaderObserver::Unbind(GDIType GDI) const
    {
        switch (GDI.Value)
        {
        case GDIType::None:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return;

        case GDIType::OpenGL:
            Get<OpenGLShader>().Unbind();
            return;
        }
    }

    void ShaderUser::Release() const
    {
        auto gdi = Renderer::GetActiveGDItype();
        switch (gdi.Value)
        {
        case GDIType::None:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return;

        case GDIType::OpenGL:
            if (AllOf<OpenGLShader>())
                Erase<OpenGLShader>();
            return;
        }
    }

    void ShaderUser::SendDataToGPU(GDIType GDI, void* data)
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

    uint32_t ShaderObserver::GetRendererID(GDIType GDI) const
    {
        switch (GDI.Value)
        {
        case GDIType::None:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return 0;

        case GDIType::OpenGL:
            return Get<OpenGLShader>().GetProgramID();
            break;
        }
        return 0;
    }

    void ShaderObserver::UploadUniform(GDIType GDI, const Uniform& uniform, const void* dataPointer, uint32_t count, bool transpose) const
    {
        switch (GDI.Value)
        {
        case GDIType::None:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return;

        case GDIType::OpenGL:
            Get<OpenGLShader>().UploadUniform(uniform, dataPointer, count, transpose);
            return;
        }
    }

    void ShaderObserver::BindTextureSlot(GDIType GDI, const ShaderTextureSlot& textureSlot, RenderTextureSlotID* rendererTextureSlot, uint32_t count) const
    {
        switch (GDI.Value)
        {
        case GDIType::None:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return;

        case GDIType::OpenGL:
            Get<OpenGLShader>().BindTextureSlot(textureSlot, rendererTextureSlot, count);
            return;
        }
    }

    void ShaderObserver::BindTextureSlot(GDIType GDI, const ShaderTextureSlot& textureSlot, RenderTextureSlotID rendererTextureSlot) const
    {
        switch (GDI.Value)
        {
        case GDIType::None:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return;

        case GDIType::OpenGL:
            Get<OpenGLShader>().BindTextureSlot(textureSlot, rendererTextureSlot);
            return;
        }
    }

}