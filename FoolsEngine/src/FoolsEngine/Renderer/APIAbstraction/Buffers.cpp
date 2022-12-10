#include "FE_pch.h"

#include "Buffers.h"
#include "FoolsEngine\Platform\OpenGL\OpenGLBuffers.h"
#include "FoolsEngine\Renderer\Renderer.h"

namespace fe
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
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

	SDPrimitive SDPrimitiveInType(SDType type)
	{
		if ((int)type > 29 || (int)type <= 0) {
			FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
			return SDPrimitive::None;
		}

		const static SDPrimitive SDPrimitiveInTypeLookupTable[] = {
			SDPrimitive::None,
			SDPrimitive::Bool,   SDPrimitive::Bool,   SDPrimitive::Bool,   SDPrimitive::Bool,
			SDPrimitive::Int,    SDPrimitive::Int,    SDPrimitive::Int,    SDPrimitive::Int,
			SDPrimitive::UInt,   SDPrimitive::UInt,	  SDPrimitive::UInt,   SDPrimitive::UInt,
			SDPrimitive::Float,  SDPrimitive::Float,  SDPrimitive::Float,  SDPrimitive::Float,
			SDPrimitive::Double, SDPrimitive::Double, SDPrimitive::Double, SDPrimitive::Double,
			SDPrimitive::Float,  SDPrimitive::Float,  SDPrimitive::Float,
			SDPrimitive::Float,  SDPrimitive::Float,  SDPrimitive::Float,
			SDPrimitive::Float,  SDPrimitive::Float,  SDPrimitive::Float
		};
		
		return SDPrimitiveInTypeLookupTable[(int)type];
	}

	SDStructure SDStructureInType(SDType type)
	{
		if ((int)type > 29 || (int)type <= 0) {
			FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
			return SDStructure::None;
		}

		const static SDStructure SDStructureInTypeLookupTable[] = {
			 SDStructure::None,
			 SDStructure::Scalar, SDStructure::Vector, SDStructure::Vector, SDStructure::Vector,
			 SDStructure::Scalar, SDStructure::Vector, SDStructure::Vector, SDStructure::Vector,
			 SDStructure::Scalar, SDStructure::Vector, SDStructure::Vector, SDStructure::Vector,
			 SDStructure::Scalar, SDStructure::Vector, SDStructure::Vector, SDStructure::Vector,
			 SDStructure::Scalar, SDStructure::Vector, SDStructure::Vector, SDStructure::Vector,
			 SDStructure::Matrix2, SDStructure::Matrix2, SDStructure::Matrix2,
			 SDStructure::Matrix3, SDStructure::Matrix3, SDStructure::Matrix3,
			 SDStructure::Matrix4, SDStructure::Matrix4, SDStructure::Matrix4
		};

		return SDStructureInTypeLookupTable[(int)type];
	}

	static uint32_t SDSizeOfType(SDType type)
	{
		const static uint32_t SDSizeOfTypeLookupTable[] = {
			0,
			4, 4*2, 4*3, 4*4,
			4, 4*2, 4*3, 4*4,
			4, 4*2, 4*3, 4*4,
			4, 4*2, 4*3, 4*4,
			8, 8*2, 8*3, 8*4,
			4*2*2, 4*2*3, 4*2*4,
			4*3*2, 4*3*3, 4*3*4,
			4*4*2, 4*4*3, 4*4*4
		};

		if ((int)type >= 29 || (int)type < 0) {
			FE_CORE_ASSERT(false, "Uknown Shader Data Type!");
			return 0;
		}

		return SDSizeOfTypeLookupTable[(int)type];
	}

	const SDType BufferElement::m_SDTypeOfMatrixLookupTable[3][3] =	{
		{ SDType::Mat2,   SDType::Mat2x3, SDType::Mat2x4 },
		{ SDType::Mat3x2, SDType::Mat3,   SDType::Mat3x4 },
		{ SDType::Mat4x2, SDType::Mat4x3, SDType::Mat4   }
	};

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

	BufferElement::BufferElement(const std::string& name, int collumns, int rows, bool normalized)
		: Name(name), Primitive(SDPrimitive::Float), Offset(0), Normalized(normalized)
	{
		Type = m_SDTypeOfMatrixLookupTable[collumns-2][rows-2];
		Size = SDSizeOfType(Type);
		ComponentCount = collumns * rows;
		Structure = SDStructureInType(Type);
	}

	BufferElement::BufferElement(const std::string& name, int collumns, uint32_t rows, bool normalized)
		: Name(name), Primitive(SDPrimitive::Float), Offset(0), Normalized(normalized)
	{
		Type = m_SDTypeOfMatrixLookupTable[collumns - 2][rows - 2];
		Size = SDSizeOfType(Type);
		ComponentCount = collumns * rows;
		Structure = SDStructureInType(Type);
	}

	BufferElement::BufferElement(const std::string& name, int order, bool normalized)
		: Name(name), Primitive(SDPrimitive::Float), Offset(0), Normalized(normalized)
	{
		Type = m_SDTypeOfMatrixLookupTable[order - 2][order - 2];
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

}