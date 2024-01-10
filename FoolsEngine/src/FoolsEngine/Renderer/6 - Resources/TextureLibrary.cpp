#include "FE_pch.h"
#include "TextureLibrary.h"

namespace fe
{
	TextureLibrary* TextureLibrary::s_ActiveInstance = nullptr;

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