#pragma once

#include <memory>
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

		const TextureData::Type GetType()  const { return m_Type; }
		const std::string&      GetName()  const { return m_Name; }
		const uint32_t          GetCount() const { return m_Count; }
	private:
		TextureData::Type m_Type;
		std::string       m_Name;
		uint32_t          m_Count;
	};
}