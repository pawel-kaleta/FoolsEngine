#include "FE_pch.h"

#include "ShaderLibrary.h"

namespace fe
{
	ShaderLibrary* ShaderLibrary::s_ActiveInstance = nullptr;

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