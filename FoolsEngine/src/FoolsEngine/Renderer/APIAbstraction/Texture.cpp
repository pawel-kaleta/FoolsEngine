#include "FE_pch.h"

#include "Texture.h"
#include "FoolsEngine\Renderer\Renderer.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLTexture.h"

namespace fe
{
	Scope<Texture> Texture::s_DefaultTexture = Scope<Texture>();

	Scope<Texture> Texture2D::Create(const std::string& filePath)
	{
		switch (Renderer::GetGDItype())
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLTexture2D>(filePath);
		}

		FE_CORE_ASSERT(false, "Unknown GDI");
		return nullptr;
	}
}