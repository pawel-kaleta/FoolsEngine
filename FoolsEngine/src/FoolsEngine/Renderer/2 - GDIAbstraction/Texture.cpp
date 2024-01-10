#include "FE_pch.h"

#include "Texture.h"
#include "OpenGL\OpenGLTexture.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

namespace fe
{
	Scope<Texture> Texture2D::Create(const std::string& name, uint32_t width, uint32_t hight)
	{
		return Create(name, width, hight, Renderer::GetActiveGDItype());
	}

	Scope<Texture> Texture2D::Create(const std::string& name, uint32_t width, uint32_t hight, GDIType GDI)
	{
		switch (GDI)
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLTexture2D>(name, width, hight);
		}

		FE_CORE_ASSERT(false, "Unknown GDI");
		return nullptr;
	}

	Scope<Texture> Texture2D::Create(const std::string& filePath)
	{
		return Create(filePath, Renderer::GetActiveGDItype());
	}

	Scope<Texture> Texture2D::Create(const std::string& filePath, GDIType GDI)
	{
		switch (GDI)
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