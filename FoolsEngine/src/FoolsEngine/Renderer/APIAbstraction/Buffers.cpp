#include "FE_pch.h"

#include "Buffers.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLBuffers.h"
#include "FoolsEngine\Renderer\Renderer.h"

namespace fe
{
	BufferElement::BufferElement(const std::string& name, ShaderData::Primitive primitive, bool normalized)
		: Name(name), Primitive(primitive), Structure(ShaderData::Structure::Scalar), Offset(0), ComponentCount(1), Normalized(normalized)
	{
		Type = (ShaderData::Type)(((int)primitive - 1) * 4 + 1);
		Size = ShaderData::SizeOfType(Type);
	}
	BufferElement::BufferElement(const std::string& name, ShaderData::Primitive primitive, int count, bool normalized)
		: Name(name), Primitive(primitive), Offset(0), ComponentCount(count), Normalized(normalized)
	{
		Type = (ShaderData::Type)(((int)primitive - 1) * 4 + count);
		Size = ShaderData::SizeOfType(Type);
		Structure = ShaderData::StructureInType(Type);
	}
	BufferElement::BufferElement(const std::string& name, ShaderData::Primitive primitive, uint32_t count, bool normalized)
		: Name(name), Primitive(primitive), Offset(0), ComponentCount(count), Normalized(normalized)
	{
		Type = (ShaderData::Type)(((int)primitive - 1) * 4 + count);
		Size = ShaderData::SizeOfType(Type);
		Structure = ShaderData::StructureInType(Type);
	}
	BufferElement::BufferElement(const std::string& name, int rows, int columns, bool normalized)
		: Name(name), Primitive(ShaderData::Primitive::Float), Offset(0), Normalized(normalized)
	{
		Type = ShaderData::TypeOfMatrix(rows, columns);
		Size = ShaderData::SizeOfType(Type);
		ComponentCount =  rows * columns;
		Structure = ShaderData::StructureInType(Type);
	}
	BufferElement::BufferElement(const std::string& name, int order, bool normalized)
		: Name(name), Primitive(ShaderData::Primitive::Float), Offset(0), Normalized(normalized)
	{
		Type = ShaderData::TypeOfMatrix(order, order);
		Size = ShaderData::SizeOfType(Type);
		ComponentCount = order * order;
		Structure = ShaderData::StructureInType(Type);
	}
	BufferElement::BufferElement(const std::string& name, ShaderData::Type type, bool normalized)
		: Name(name), Type(type), Offset(0), Normalized(normalized)
	{
		Primitive = ShaderData::PrimitiveInType(Type);
		Structure = ShaderData::StructureInType(Type);
		Size = ShaderData::SizeOfType(Type);
		bool ifDouble = Primitive == ShaderData::Primitive::Double;
		ComponentCount = Size / (4 * (1 + (int)ifDouble) );

	}
	BufferElement::BufferElement(ShaderData::Type type, const std::string& name, bool normalized)
		: Name(name), Type(type), Offset(0), Normalized(normalized)
	{
		Primitive = ShaderData::PrimitiveInType(Type);
		Structure = ShaderData::StructureInType(Type);
		Size = ShaderData::SizeOfType(Type);
		bool ifDouble = Primitive == ShaderData::Primitive::Double;
		ComponentCount = Size / (4 * (1 + (int)ifDouble) );
	}

	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
		: m_Elements(elements)
	{
		FE_PROFILER_FUNC();
		CalculateOffsetsAndStride();
	}

	void BufferLayout::CalculateOffsetsAndStride()
	{
		uint32_t offset = 0;
		m_Stride = 0;

		for (auto& element : m_Elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}

	Scope<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		FE_PROFILER_FUNC();
		switch (Renderer::GetGDItype())
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLVertexBuffer>(vertices, size);
		}

		FE_CORE_ASSERT(false, "Unknown GDI");
		return nullptr;
	}
	
	Scope<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		FE_PROFILER_FUNC();
		switch (Renderer::GetGDItype())
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return CreateScope<OpenGLIndexBuffer>(indices, count);
		}

		FE_CORE_ASSERT(false, "Unknown GDI");
		return nullptr;
	}

}