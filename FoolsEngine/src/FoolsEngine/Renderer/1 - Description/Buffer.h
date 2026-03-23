#pragma once

#include "Data.h"

#include "FoolsEngine/Foundation/Memory/String.h"
#include "FoolsEngine/Application/UUID.h"

#include <glm/glm.hpp>

namespace fe
{
	namespace Description::Buffer
	{
		FE_DECLARE_ENUM(Usage, None, Vertex, Index, IndexVertex, Batch, Uniform, ShaderStorage); // do we need this?

		struct Element
		{
			String Name;
			Data::Type Type = Data::Type::None;
			U32 Count = 1; // is count handled everywhere properly?
			bool Normalized = 0; // TO DO: do we need this?

			Data::Primitive Primitive() const { return Data::PrimitiveInType(Type); }
			Data::Structure Structure() const { return Data::StructureInType(Type); }
			UInt Size() const { return Data::SizeOfType(Type); }
			UInt ComponentCount() const
			{
				bool ifDouble = Primitive() == Data::Primitive::Double;
				return Size() / (4 * (1 + (int)ifDouble));
			}
		};

		FE_DECLARE_ENUM(LayoutType, None, Vertex, Index, MainUniforms, STD140, Internal);
		// Vertex - vertex buffer
		// Index - index buffer
		// MainUniforms - individually bound and uploaded uniforms
		// STD140 - uniform buffers / shader storage buffers
		// Internal - vertex output / fragment input / fragment output

		struct Layout
		{
		public:
			Splice<Element> Elements;
			Splice<U32> Offsets;
			
			UUID UUID = fe::UUID();
			LayoutType Type = LayoutType::None;
			U32 Stride;

			void CalculateOffsetsAndStride();
		};

		struct UniformBufferIterator
		{
			UniformBufferIterator(Splice<Element> elements, void* data)
				: m_Elements(elements), m_CurrentData((Byte*)data), m_Index(0) { }

			void Move()
			{
				const auto& element = m_Elements[m_Index];
				m_CurrentData += element.Size() * element.Count;
			}

			bool IsEnd() { if (m_Index >= m_Elements.Count) return true; }

			void* Get() { return m_CurrentData; }

			Splice<Element> m_Elements;
			Byte* m_CurrentData;
			UInt m_Index;
		};

		struct Vertex {
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec3 Tangent;
			glm::vec2 UV0;
			glm::vec2 UV1;

			static const Layout& GetLayout();
		};
	}
}