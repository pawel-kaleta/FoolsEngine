#pragma once
#include "FE_pch.h"
#include "FoolsEngine\Renderer\ShaderData.h"

namespace fe
{
	class BufferElement
	{
	public:
		std::string Name;
		ShaderData::Primitive Primitive;
		ShaderData::Structure Structure;
		ShaderData::Type Type;
		uint32_t Size;
		uint32_t Offset;
		uint32_t ComponentCount;
		bool Normalized;

		BufferElement(const std::string& name, ShaderData::Primitive primitive,                 bool normalized = false);
		BufferElement(const std::string& name, ShaderData::Primitive primitive, int count,      bool normalized = false);
		BufferElement(const std::string& name, ShaderData::Primitive primitive, uint32_t count, bool normalized = false);
		BufferElement(const std::string& name, int rows, int columns,                           bool normalized = false);
		BufferElement(const std::string& name, int order,                                       bool normalized = false);
		BufferElement(const std::string& name, ShaderData::Type type,                           bool normalized = false);
		
		BufferElement(ShaderData::Type type, const std::string& name, bool normalized = false);

	private:

	};

	class BufferLayout
	{
	public:
		BufferLayout() {};
		BufferLayout(const std::initializer_list<BufferElement>& elements);

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
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetSize() const  = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}