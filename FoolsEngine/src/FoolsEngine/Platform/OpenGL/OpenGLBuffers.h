#pragma once

#include "FoolsEngine\Renderer\Buffers.h"
#include <glad\glad.h>

namespace fe
{
	class OpenGLVertexBuffer : VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetSize() const override;

		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		virtual const BufferLayout& GetLayout() override { return m_Layout;  }

	private:
		uint32_t m_ID;
		uint32_t m_Size;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override;

	private:
		uint32_t m_ID;
		uint32_t m_Count;
	};

	static GLenum SDPrimitiveToGLBaseType(SDPrimitive primitive)
	{
		const static GLenum LookupTable[] = { GL_BOOL, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_DOUBLE };
		return LookupTable[(int)primitive - 1];
	};
}