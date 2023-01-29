#include "FE_pch.h"

#include "Texture.h"
#include "FoolsEngine\Renderer\Renderer.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLTexture.h"

namespace fe
{
	TextureLibrary* TextureLibrary::s_ActiveInstance = nullptr;

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

	void TextureLibrary::IAdd(const Ref<Texture> texture)
	{
		const std::string& name = texture->GetName();
		FE_CORE_ASSERT(!IExist(name), "Texture already in library!");
		m_Textures[name] = texture;
	}

	bool TextureLibrary::IExist(const std::string& name) const
	{
		return m_Textures.find(name) != m_Textures.end();;
	}

	Ref<Texture> TextureLibrary::IGet(const std::string& name)
	{
		FE_CORE_ASSERT(IExist(name), "Texture not found!");
		return m_Textures[name];
	}
}