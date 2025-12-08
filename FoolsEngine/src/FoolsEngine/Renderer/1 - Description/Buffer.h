#pragma once

#include "Data.h"

#include <glm/glm.hpp>

namespace fe
{
	namespace Description::Buffer
	{
		FE_DECLARE_ENUM(Usage, None, Vertex, Index, Batch, Uniform, ShaderStorage);

		struct Element
		{
			std::pmr::string Name;
			Data::Type Type;
			uint32_t Count;
			bool Normalized; // TO DO: do we need this?

			Element();
			Element(Data::Type type, const std::string& name, uint32_t count = 1, bool normalized = false);

			Data::Primitive Primitive() const { return Data::PrimitiveInType(Type); }
			Data::Structure Structure() const { return Data::StructureInType(Type); }
			size_t Size() const { return Data::SizeOfType(Type); }
			size_t ComponentCount() const
			{
				bool ifDouble = Primitive() == Data::Primitive::Double;
				return Size() / (4 * (1 + (int)ifDouble));
			}
		};

		FE_DECLARE_ENUM(LayoutType, None, Vertex, Index, MainUniforms, STD140, Internal);
		// Vertex - vertex buffer
		// MainUniforms - individually bound and uploaded uniforms
		// STD140 - uniform buffers / shader storage buffers
		// Internal - vertex output / fragment input / fragment output

		struct Layout
		{
		public:
			std::pmr::vector<Element> Elements;
			std::pmr::vector<uint32_t> Offsets;
				
			LayoutType Type;
			uint32_t Stride;

			Layout();
			void CalculateOffsetsAndStride();
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