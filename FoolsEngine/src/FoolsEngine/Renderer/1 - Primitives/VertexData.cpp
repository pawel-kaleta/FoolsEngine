#include "FE_pch.h"
#include "VertexData.h"

namespace fe
{
	namespace VertexData
	{
		Element::Element(const std::string& name, ShaderData::Primitive primitive, bool normalized)
			: Name(name), Offset(0), ComponentCount(1), Normalized(normalized)
		{
			Type.FromInt((primitive.ToInt() - 1) * 4 + 1);
		}

		Element::Element(const std::string& name, ShaderData::Primitive primitive, int count, bool normalized)
			: Name(name), Offset(0), ComponentCount(count), Normalized(normalized)
		{
			Type.FromInt((primitive.ToInt() - 1) * 4 + count);
		}

		Element::Element(const std::string& name, ShaderData::Primitive primitive, uint32_t count, bool normalized)
			: Name(name), Offset(0), ComponentCount(count), Normalized(normalized)
		{
			Type.FromInt((primitive.ToInt() - 1) * 4 + count);
		}

		Element::Element(const std::string& name, int rows, int columns, bool normalized)
			: Name(name), Offset(0), ComponentCount(rows * columns), Normalized(normalized), Type(ShaderData::TypeOfMatrix(rows, columns))
		{ }

		Element::Element(const std::string& name, int order, bool normalized)
			: Name(name), Offset(0), ComponentCount(order* order), Normalized(normalized), Type(ShaderData::TypeOfMatrix(order, order))
		{ }

		Element::Element(const std::string& name, ShaderData::Type type, bool normalized)
			: Name(name), Offset(0), Normalized(normalized), Type(type)
		{
			bool ifDouble = Primitive() == ShaderData::Primitive::Double;
			ComponentCount = (uint32_t)Size() / (4 * (1 + (int)ifDouble));
		}
		Element::Element(ShaderData::Type type, const std::string& name, bool normalized)
			: Name(name), Offset(0), Normalized(normalized), Type(type)
		{
			bool ifDouble = Primitive() == ShaderData::Primitive::Double;
			ComponentCount = (uint32_t)Size() / (4 * (1 + (int)ifDouble));
		}

		Layout::Layout(const std::initializer_list<Element>& elements)
			: m_Elements(elements)
		{
			FE_PROFILER_FUNC();
			CalculateOffsetsAndStride();
		}

		void Layout::CalculateOffsetsAndStride()
		{
			size_t offset = 0;
			m_Stride = 0;

			for (auto& element : m_Elements)
			{
				element.Offset = (uint32_t)offset;
				offset += element.Size();
				m_Stride += (uint32_t)element.Size();
			}
		}
	}
}