#include "FE_pch.h"
#include "Shader.h"
#include "FoolsEngine\Renderer\Renderer.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLShader.h"

namespace fe
{
    ShaderLibrary* ShaderLibrary::s_ActiveInstance = nullptr;

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


    void ShaderLibrary::IAdd(const Ref<Shader> shader)
    {
        const std::string& name = shader->GetName();
        FE_CORE_ASSERT(!IExist(name), "Shader already in library!");
        m_Shaders[name] = shader;
    }

    Ref<Shader> ShaderLibrary::ILoad(const std::string& filePath)
    {
        Ref<Shader> shader = Shader::Create(filePath);
        IAdd(shader);
        return shader;
    }

    bool ShaderLibrary::IExist(const std::string& name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();
    }

    Ref<Shader> ShaderLibrary::IGet(const std::string& name)
    {
        FE_CORE_ASSERT(IExist(name), "Shader not found!");
        return m_Shaders[name];
    }

}