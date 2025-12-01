#pragma once

#include <string>

#include "Texture.h"

namespace fe
{
	class ShaderTextureSlot
	{
	public:
		ShaderTextureSlot()
			: m_Type(Description::Texture::Type::None), m_Count(0) {}
		ShaderTextureSlot(const std::string& name, Description::Texture::Type type, uint32_t count = 1)
			: m_Name(name), m_Type(type), m_Count(count) {}

		Description::Texture::Type m_Type;
		std::string	m_Name;
		uint32_t	m_Count;
	};
}