#pragma once

#include <string>
#include <vector>

#include "ShaderData.h"

namespace fe
{
	namespace VertexData
	{
		struct Element
		{
			std::string Name;
			ShaderData::Primitive Primitive;
			ShaderData::Structure Structure;
			ShaderData::Type Type;
			uint32_t Size;
			uint32_t Offset;
			uint32_t ComponentCount;
			bool Normalized;

			Element(const std::string& name, ShaderData::Primitive primitive, bool normalized = false);
			Element(const std::string& name, ShaderData::Primitive primitive, int count, bool normalized = false);
			Element(const std::string& name, ShaderData::Primitive primitive, uint32_t count, bool normalized = false);
			Element(const std::string& name, int rows, int columns, bool normalized = false);
			Element(const std::string& name, int order, bool normalized = false);
			Element(const std::string& name, ShaderData::Type type, bool normalized = false);

			Element(ShaderData::Type type, const std::string& name, bool normalized = false);
		};

		class Layout
		{
		public:
			Layout() {};
			Layout(const std::initializer_list<Element>& elements);

			const std::vector<Element>& GetElements() const { return m_Elements; }
			const uint32_t              GetStride()   const { return m_Stride; }

			std::vector<Element>::iterator begin() { return m_Elements.begin(); }
			std::vector<Element>::iterator end()   { return m_Elements.end(); }

			std::vector<Element>::const_iterator begin() const { return m_Elements.begin(); }
			std::vector<Element>::const_iterator end()   const { return m_Elements.end(); }

		private:
			std::vector<Element> m_Elements;
			uint32_t m_Stride = 0;

			void CalculateOffsetsAndStride();
		};
	}
}