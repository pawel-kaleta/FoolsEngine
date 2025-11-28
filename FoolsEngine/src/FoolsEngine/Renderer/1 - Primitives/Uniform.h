#pragma once

#include <string>

#include "ShaderData.h"

namespace fe
{
	class Uniform
	{
	public:
		Uniform(const std::string& name, ShaderData::Type type, uint32_t count = 1)
			: m_Name(name), m_Type(type), m_Count(count) {}

		Uniform()
			: m_Type(ShaderData::Type::None), m_Count(0) {}

		const ShaderData::Structure GetStructure() const { return ShaderData::StructureInType(m_Type); }
		const ShaderData::Primitive GetPrimitive() const { return ShaderData::PrimitiveInType(m_Type); }
		const size_t                GetSize()      const { return ShaderData::SizeOfType(m_Type) * m_Count; }

		std::string      m_Name;
		ShaderData::Type m_Type;
		uint32_t         m_Count;
	};
}