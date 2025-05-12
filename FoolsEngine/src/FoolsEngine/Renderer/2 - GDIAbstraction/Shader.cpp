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
    void Shader::Bind(GDIType GDI)
    {
        switch (GDI)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return;

        case GDIType::OpenGL:
            Get<OpenGLShader>().Bind();
            return;
        }
    }

    void Shader::Unbind(GDIType GDI)
    {
        switch (GDI)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return;

        case GDIType::OpenGL:
            Get<OpenGLShader>().Unbind();
            return;
        }
    }

    void Shader::Release()
    {
        auto gdi = Renderer::GetActiveGDItype();
        switch (gdi)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return;

        case GDIType::OpenGL:
            if (AllOf<OpenGLShader>())
                Erase<OpenGLShader>();
            return;
        }
    }

    void Shader::UnloadFromCPU()
    {
        auto& dataPtr = GetDataLocation().Data;
        if (dataPtr)
        {
            free(dataPtr);
            dataPtr = nullptr;
        }
        auto& sourceCode = Get<ACSourceCode>();
        sourceCode.ShaderSource.clear();
        sourceCode.VertexSource.clear();
        sourceCode.FragmentSource.clear();
    }

    uint32_t Shader::GetRendererID(GDIType GDI) const
    {
        switch (GDI)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return 0;

        case GDIType::OpenGL:
            return Get<OpenGLShader>().GetProgramID();
            break;
        }
        return 0;
    }

    void Shader::UploadUniform(GDIType GDI, const Uniform& uniform, void* dataPointer, uint32_t count, bool transpose)
    {
        switch (GDI)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return;

        case GDIType::OpenGL:
            Get<OpenGLShader>().UploadUniform(uniform, dataPointer, count, transpose);
            return;
        }
    }

    void Shader::BindTextureSlot(GDIType GDI, const ShaderTextureSlot& textureSlot, RenderTextureSlotID* rendererTextureSlot, uint32_t count)
    {
        switch (GDI)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return;

        case GDIType::OpenGL:
            Get<OpenGLShader>().BindTextureSlot(textureSlot, rendererTextureSlot, count);
            return;
        }
    }

    void Shader::BindTextureSlot(GDIType GDI, const ShaderTextureSlot& textureSlot, RenderTextureSlotID rendererTextureSlot)
    {
        switch (GDI)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "Unspecified GDIType");
            return;

        case GDIType::OpenGL:
            Get<OpenGLShader>().BindTextureSlot(textureSlot, rendererTextureSlot);
            return;
        }
    }
}