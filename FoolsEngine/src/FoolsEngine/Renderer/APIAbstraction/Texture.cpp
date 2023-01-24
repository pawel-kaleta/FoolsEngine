#include "FE_pch.h"

#include "Texture.h"
#include "FoolsEngine\Renderer\Renderer.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLTexture.h"

namespace fe
{
	Scope<Texture> Texture::s_DefaultTexture = Scope<Texture>();

	Scope<Texture> Texture2D::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPItype())
		{
		case RenderCommands::APItype::none:
			FE_CORE_ASSERT(false, "Renderer::RendererAPI::none currently not supported!");
			return nullptr;
		case RenderCommands::APItype::OpenGL:
			return CreateScope<OpenGLTexture2D>(filePath);
		}

		FE_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}
}