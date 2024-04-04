#include "FE_pch.h"

#include "Shader.h"
#include "OpenGL\OpenGLShader.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

namespace fe
{
    Shader* fe::Shader::Create(const AssetSignature& assetSignature, const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
    {
        return Create(assetSignature, name, vertexSource, fragmentSource, Renderer::GetActiveGDItype());
    }

    Shader* fe::Shader::Create(const AssetSignature& assetSignature, const std::string& name, const std::string& vertexSource, const std::string& fragmentSource, GDIType GDI)
    {
        switch (GDI)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "GDItype::none currently not supported!");
            return nullptr;
        case GDIType::OpenGL:
            return new OpenGLShader(assetSignature, name, vertexSource, fragmentSource);
        }

        FE_CORE_ASSERT(false, "Unknown GDI");
        return nullptr;
    }

    Shader* fe::Shader::Create(const AssetSignature& assetSignature, const std::string& name, const std::string& shaderSource)
    {
        return Create(assetSignature, name, shaderSource, Renderer::GetActiveGDItype());
    }

    Shader* fe::Shader::Create(const AssetSignature& assetSignature, const std::string& name, const std::string& shaderSource, GDIType GDI)
    {
        switch (GDI)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
            return nullptr;
        case GDIType::OpenGL:
            return new OpenGLShader(assetSignature, name, shaderSource);
        }

        FE_CORE_ASSERT(false, "Unknown GDI");
        return nullptr;
    }

    Shader* fe::Shader::Create(const AssetSignature& assetSignature, const std::string& filePath)
    {
        return Create(assetSignature, filePath, Renderer::GetActiveGDItype());
    }

    Shader* fe::Shader::Create(const AssetSignature& assetSignature, const std::string& filePath, GDIType GDI)
    {
        switch (GDI)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
            return nullptr;
        case GDIType::OpenGL:
            return new OpenGLShader(assetSignature, filePath);
        }

        FE_CORE_ASSERT(false, "Unknown GDI");
        return nullptr;
    }
}