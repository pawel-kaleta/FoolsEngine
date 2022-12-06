#include "FE_pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLShader.h"

namespace fe
{
    Shader* fe::Shader::Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
    {
        switch (Renderer::GetAPI())
        {
        case Renderer::RendererAPI::none:
            FE_CORE_ASSERT(false, "Renderer::RendererAPI::none currently not supported!");
            return nullptr;
        case Renderer::RendererAPI::OpenGL:
            return (Shader*) new OpenGLShader(name, vertexSource, fragmentSource);
        }

        FE_CORE_ASSERT(false, "Unknown renderer API");
        return nullptr;
    }

}