#pragma once

#include <memory>
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

		const std::string& GetName() const { return m_Name; }
		const ShaderData::Type& GetType() const { return m_Type; }
		const ShaderData::Structure GetStructure() const { return ShaderData::StructureInType(m_Type); }
		const ShaderData::Primitive GetPrimitive() const { return ShaderData::PrimitiveInType(m_Type); }
		const uint32_t GetSize() const { return SizeOfType(m_Type) * m_Count; }
		const uint32_t GetCount() const { return m_Count; }
	private:
		std::string m_Name;
		ShaderData::Type m_Type;
		uint32_t m_Count;
	};
}