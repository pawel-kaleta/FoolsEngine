#pragma once

#include "FoolsEngine\Renderer\Buffers.h"

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

	private:
		uint32_t m_ID;
		uint32_t m_Size;

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
}