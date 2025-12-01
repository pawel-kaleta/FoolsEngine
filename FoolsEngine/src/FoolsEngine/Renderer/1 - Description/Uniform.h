#pragma once

#include <string>

#include "Data.h"

namespace fe
{
	class Uniform
	{
	public:
		Uniform(const std::string& name, Description::Data::Type type, uint32_t count = 1)
			: m_Name(name), m_Type(type), m_Count(count) {}

		Uniform()
			: m_Type(Description::Data::Type::None), m_Count(0) {}

		const Description::Data::Structure GetStructure()	const { return Description::Data::StructureInType(m_Type); }
		const Description::Data::Primitive GetPrimitive()	const { return Description::Data::PrimitiveInType(m_Type); }
		const size_t                GetSize()				const { return Description::Data::SizeOfType(m_Type) * m_Count; }

		Description::Data::Type m_Type;
		std::string	m_Name;
		uint32_t	m_Count;
	};
}