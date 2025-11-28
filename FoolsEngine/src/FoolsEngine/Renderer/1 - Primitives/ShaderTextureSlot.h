#pragma once

#include <string>

#include "TextureData.h"

namespace fe
{
	class ShaderTextureSlot
	{
	public:
		ShaderTextureSlot()
			: m_Type(TextureData::Type::None), m_Count(0) {}
		ShaderTextureSlot(const std::string& name, TextureData::Type type, uint32_t count = 1)
			: m_Name(name), m_Type(type), m_Count(count) {}


		std::string       m_Name;
		TextureData::Type m_Type;
		uint32_t          m_Count;
	};
}