#pragma once
#include "FE_pch.h"

namespace fe
{
	enum class SDPrimitive
	{
		None = 0,
		Bool,
		Int,
		UInt,
		Float,
		Double
	};

	enum class SDStructure
	{
		None = 0,
		Scalar,
		Vector,
		Matrix2, //2x2, 2x3, 2x4
		Matrix3, //3x2, 3x3, 3x4
		Matrix4  //4x2, 4x3, 4x4
	};
	
	enum class SDType
	{
		None = 0,
		Bool,   Bool2,   Bool3,   Bool4,
		Int,    Int2,    Int3,    Int4,
		UInt,   UInt2,   UInt3,   UInt4,
		Float,  Float2,  Float3,  Float4,
		Double, Double2, Double3, Double4,
		Mat2,   Mat2x3, Mat2x4,
		Mat3x2, Mat3,   Mat3x4,
		Mat4x2, Mat4x3, Mat4
	};

	static SDPrimitive SDPrimitiveInType(SDType type);
	static SDStructure SDStructureInType(SDType type);	
	
	static uint32_t SDSizeOfType(SDType type);

	class BufferElement
	{
	public:
		std::string Name;
		SDPrimitive Primitive;
		SDStructure Structure;
		SDType Type;
		uint32_t Size;
		uint32_t Offset;
		uint32_t ComponentCount;
		bool Normalized;

		BufferElement(const std::string& name,	SDPrimitive primitive,					bool normalized = false);
		BufferElement(const std::string& name,	SDPrimitive primitive, int count,		bool normalized = false);
		BufferElement(const std::string& name,	SDPrimitive primitive, uint32_t count,	bool normalized = false);
		BufferElement(const std::string& name,	int columns, int rows,					bool normalized = false);
		BufferElement(const std::string& name,	int columns, uint32_t rows,				bool normalized = false);
		BufferElement(const std::string& name,	int order,								bool normalized = false);
		BufferElement(const std::string& name,	SDType type,							bool normalized = false);
		
		BufferElement(SDType type, const std::string& name, bool normalized = false);

	private:
		const static SDType m_SDTypeOfMatrixLookupTable[3][3];
	};


	class BufferLayout
	{
	public:
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		//
		BufferLayout() {};
		//

		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		inline const uint32_t GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;

		void CalculateOffsetsAndStride();
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {};

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetSize() const = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() = 0;

		static VertexBuffer* Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {};

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static IndexBuffer* Create(uint32_t* indices, uint32_t count);
	};
}