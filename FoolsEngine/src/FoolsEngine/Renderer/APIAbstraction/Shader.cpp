#include "FE_pch.h"
#include "Shader.h"
#include "FoolsEngine\Renderer\Renderer.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLShader.h"

namespace fe
{
    Ref<Shader> fe::Shader::Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
    {
        switch (RendererAPI::GetNativeAPI())
        {
        case RendererAPI::NativeAPI::none:
            FE_CORE_ASSERT(false, "Renderer::NativeAPI::none currently not supported!");
            return nullptr;
        case RendererAPI::NativeAPI::OpenGL:
            return CreateRef<OpenGLShader>(name, vertexSource, fragmentSource);
        }

        FE_CORE_ASSERT(false, "Unknown renderer API");
        return nullptr;
    }

}