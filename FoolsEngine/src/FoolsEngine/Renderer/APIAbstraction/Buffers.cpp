#include "FE_pch.h"

#include "Buffers.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLBuffers.h"
#include "FoolsEngine\Renderer\Renderer.h"

namespace fe
{
	BufferElement::BufferElement(const std::string& name, SDPrimitive primitive, bool normalized)
		: Name(name), Primitive(primitive), Structure(SDStructure::Scalar), Offset(0), ComponentCount(1), Normalized(normalized)
	{
		Type = (SDType)(((int)primitive - 1) * 4 + 1);
		Size = SDSizeOfType(Type);
	}
	BufferElement::BufferElement(const std::string& name, SDPrimitive primitive, int count, bool normalized)
		: Name(name), Primitive(primitive), Offset(0), ComponentCount(count), Normalized(normalized)
	{
		Type = (SDType)(((int)primitive - 1) * 4 + count);
		Size = SDSizeOfType(Type);
		Structure = SDStructureInType(Type);
	}
	BufferElement::BufferElement(const std::string& name, SDPrimitive primitive, uint32_t count, bool normalized)
		: Name(name), Primitive(primitive), Offset(0), ComponentCount(count), Normalized(normalized)
	{
		Type = (SDType)(((int)primitive - 1) * 4 + count);
		Size = SDSizeOfType(Type);
		Structure = SDStructureInType(Type);
	}
	BufferElement::BufferElement(const std::string& name, int rows, int columns, bool normalized)
		: Name(name), Primitive(SDPrimitive::Float), Offset(0), Normalized(normalized)
	{
		//Type = m_SDTypeOfMatrixLookupTable[collumns-2][rows-2];
		Type = SDTypeOfMatrix(rows, columns);
		Size = SDSizeOfType(Type);
		ComponentCount =  rows * columns;
		Structure = SDStructureInType(Type);
	}
	BufferElement::BufferElement(const std::string& name, int rows, uint32_t columns, bool normalized)
		: Name(name), Primitive(SDPrimitive::Float), Offset(0), Normalized(normalized)
	{
		//Type = m_SDTypeOfMatrixLookupTable[collumns-2][rows-2];
		Type = SDTypeOfMatrix(rows, columns);
		Size = SDSizeOfType(Type);
		ComponentCount =  rows * columns;
		Structure = SDStructureInType(Type);
	}
	BufferElement::BufferElement(const std::string& name, int order, bool normalized)
		: Name(name), Primitive(SDPrimitive::Float), Offset(0), Normalized(normalized)
	{
		//Type = m_SDTypeOfMatrixLookupTable[order-2][order-2];
		Type = SDTypeOfMatrix(order, order);
		Size = SDSizeOfType(Type);
		ComponentCount = order * order;
		Structure = SDStructureInType(Type);
	}
	BufferElement::BufferElement(const std::string& name, SDType type, bool normalized)
		: Name(name), Type(type), Offset(0), Normalized(normalized)
	{
		Primitive = SDPrimitiveInType(Type);
		Structure = SDStructureInType(Type);
		Size = SDSizeOfType(Type);
		if (Primitive == SDPrimitive::Double)
			ComponentCount = Size / 8;
		else
			ComponentCount = Size / 4;
	}
	BufferElement::BufferElement(SDType type, const std::string& name, bool normalized)
		: Name(name), Type(type), Offset(0), Normalized(normalized)
	{
		Primitive = SDPrimitiveInType(Type);
		Structure = SDStructureInType(Type);
		Size = SDSizeOfType(Type);
		if (Primitive == SDPrimitive::Double)
			ComponentCount = Size / 8;
		else
			ComponentCount = Size / 4;
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

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		FE_PROFILER_FUNC();
		switch (Renderer::GetNativeAPI())
		{
		case RendererAPI::NativeAPI::none:
			FE_CORE_ASSERT(false, "Renderer::NativeAPI::none currently not supported!");
			return nullptr;
		case RendererAPI::NativeAPI::OpenGL:
			return (VertexBuffer*) new OpenGLVertexBuffer(vertices, size);
		}

		FE_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}
	
	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		FE_PROFILER_FUNC();
		switch (RendererAPI::GetNativeAPI())
		{
		case RendererAPI::NativeAPI::none:
			FE_CORE_ASSERT(false, "Renderer::NativeAPI::none currently not supported!");
			return nullptr;
		case RendererAPI::NativeAPI::OpenGL:
			return (IndexBuffer*) new OpenGLIndexBuffer(indices, count);
		}

		FE_CORE_ASSERT(false, "Unknown renderer API");
		return nullptr;
	}

}