#pragma once

#include <string>
#include <vector>

#include "ShaderData.h"

#include <glm/glm.hpp>

namespace fe
{
	namespace VertexData
	{
		struct Element
		{
			std::string Name;
			ShaderData::Type Type;
			uint32_t Offset;
			uint32_t ComponentCount;
			bool Normalized;

			Element(ShaderData::Type type, const std::string& name, bool normalized = false)
				: Name(name), Offset(0), Normalized(normalized), Type(type)
			{
				bool ifDouble = Primitive() == ShaderData::Primitive::Double;
				ComponentCount = (uint32_t)Size() / (4 * (1 + (int)ifDouble));
			}

			ShaderData::Primitive Primitive() const { return ShaderData::PrimitiveInType(Type); }
			ShaderData::Structure Structure() const { return ShaderData::StructureInType(Type); }
			size_t Size() const { return ShaderData::SizeOfType(Type); }
		};

		class Layout
		{
		public:
			Layout() = default;
			Layout(const std::initializer_list<Element>& elements) : m_Elements(elements) { CalculateOffsetsAndStride(); }

			std::vector<Element> m_Elements;
			uint32_t m_Stride = 0;

			void CalculateOffsetsAndStride()
			{
				FE_PROFILER_FUNC();

				size_t offset = 0;
				m_Stride = 0;

				for (auto& element : m_Elements)
				{
					element.Offset = (uint32_t)offset;
					offset += element.Size();
					m_Stride += (uint32_t)element.Size();
				}
			}

			std::vector<Element>::iterator begin() { return m_Elements.begin(); }
			std::vector<Element>::iterator end() { return m_Elements.end(); }

			std::vector<Element>::const_iterator begin() const { return m_Elements.begin(); }
			std::vector<Element>::const_iterator end()   const { return m_Elements.end(); }
		};

		

		struct Vertex {
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec3 Tangent;
			glm::vec2 UV0;
			glm::vec2 UV1;

			static VertexData::Layout GetLayout() {
				return VertexData::Layout({
					{ ShaderData::Type::Float3, "a_Position" },
					{ ShaderData::Type::Float3, "a_Normal" },
					{ ShaderData::Type::Float3, "a_Tangent" },
					{ ShaderData::Type::Float2, "a_UV0" },
					{ ShaderData::Type::Float2, "a_UV1" }
				});
			}
		};
	}
}