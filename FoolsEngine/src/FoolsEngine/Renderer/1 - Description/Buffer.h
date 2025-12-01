#pragma once

#include <string>
#include <vector>

#include "Data.h"

#include <glm/glm.hpp>

namespace fe
{
	namespace Description::Buffer
	{
		struct Element
		{
			std::string Name;
			Data::Type Type;
			bool Normalized;

			Element(Data::Type type, const std::string& name, bool normalized = false)
				: Name(name), Normalized(normalized), Type(type) { }

			size_t ComponentCount() const
			{
				bool ifDouble = Primitive() == Data::Primitive::Double;
				return Size() / (4 * (1 + (int)ifDouble));
			}
			Data::Primitive Primitive() const { return Data::PrimitiveInType(Type); }
			Data::Structure Structure() const { return Data::StructureInType(Type); }
			size_t Size() const { return Data::SizeOfType(Type); }
		};

		FE_DECLARE_ENUM(LayoutType, None, Basic);

		struct Layout
		{
		public:
			Layout()
				: Type(LayoutType::None) { };
			Layout(const std::initializer_list<Element>& elements)
				: Type(LayoutType::Basic), Elements(elements) { CalculateOffsetsAndStride(); }

			std::vector<Element> Elements;
			std::vector<uint32_t> Offsets;
			LayoutType Type;
			uint32_t Stride;

			void CalculateOffsetsAndStride()
			{
				FE_PROFILER_FUNC();

				FE_CORE_ASSERT(Type == LayoutType::Basic, "Unsupported LayoutType");

				uint32_t offset = 0;
				Stride = 0;

				for (auto& element : Elements)
				{
					uint32_t size = (uint32_t)element.Size();

					Offsets.push_back(offset);

					offset += size;
					Stride += size;
				}
			}

			//std::vector<Element>::iterator begin() { return Elements.begin(); }
			//std::vector<Element>::iterator end() { return Elements.end(); }
			//
			//std::vector<Element>::const_iterator begin() const { return Elements.begin(); }
			//std::vector<Element>::const_iterator end()   const { return Elements.end(); }
		};

		struct Vertex {
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec3 Tangent;
			glm::vec2 UV0;
			glm::vec2 UV1;

			static Layout GetLayout() {
				return Layout({
					{ Description::Data::Type::Float3, "a_Position" },
					{ Description::Data::Type::Float3, "a_Normal" },
					{ Description::Data::Type::Float3, "a_Tangent" },
					{ Description::Data::Type::Float2, "a_UV0" },
					{ Description::Data::Type::Float2, "a_UV1" }
				});
			}
		};
	}
}