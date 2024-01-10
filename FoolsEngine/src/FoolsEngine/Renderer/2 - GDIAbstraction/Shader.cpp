#include "FE_pch.h"

#include "Shader.h"
#include "OpenGL\OpenGLShader.h"

#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

namespace fe
{
    Scope<Shader> fe::Shader::Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
    {
        return Create(name, vertexSource, fragmentSource, Renderer::GetActiveGDItype());
    }

    Scope<Shader> fe::Shader::Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource, GDIType GDI)
    {
        switch (GDI)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "GDItype::none currently not supported!");
            return nullptr;
        case GDIType::OpenGL:
            return CreateScope<OpenGLShader>(name, vertexSource, fragmentSource);
        }

        FE_CORE_ASSERT(false, "Unknown GDI");
        return nullptr;
    }

    Scope<Shader> fe::Shader::Create(const std::string& name, const std::string& shaderSource)
    {
        return Create(name, shaderSource, Renderer::GetActiveGDItype());
    }

    Scope<Shader> fe::Shader::Create(const std::string& name, const std::string& shaderSource, GDIType GDI)
    {
        switch (GDI)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
            return nullptr;
        case GDIType::OpenGL:
            return CreateScope<OpenGLShader>(name, shaderSource);
        }

        FE_CORE_ASSERT(false, "Unknown GDI");
        return nullptr;
    }

    Scope<Shader> fe::Shader::Create(const std::string& filePath)
    {
        return Create(filePath, Renderer::GetActiveGDItype());
    }

    Scope<Shader> fe::Shader::Create(const std::string& filePath, GDIType GDI)
    {
        switch (GDI)
        {
        case GDIType::none:
            FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
            return nullptr;
        case GDIType::OpenGL:
            return CreateScope<OpenGLShader>(filePath);
        }

        FE_CORE_ASSERT(false, "Unknown GDI");
        return nullptr;
    }


    

}