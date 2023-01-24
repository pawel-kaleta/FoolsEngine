#include "FE_pch.h"
#include "Shader.h"
#include "FoolsEngine\Renderer\Renderer.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLShader.h"

namespace fe
{
    Scope<Shader> fe::Shader::Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
    {
        switch (Renderer::GetAPItype())
        {
        case RenderCommands::APItype::none:
            FE_CORE_ASSERT(false, "Renderer::APItype::none currently not supported!");
            return nullptr;
        case RenderCommands::APItype::OpenGL:
            return CreateScope<OpenGLShader>(name, vertexSource, fragmentSource);
        }

        FE_CORE_ASSERT(false, "Unknown renderer API");
        return nullptr;
    }

}