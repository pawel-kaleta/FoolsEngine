#include "FE_pch.h"
#include "OpenGLBuffers.h"

namespace fe
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
		: m_Size(size)
	{
		FE_PROFILER_FUNC();

		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, m_Size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
		: m_Size(size)
	{
		FE_PROFILER_FUNC();

		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, m_Size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		FE_PROFILER_FUNC();
		glDeleteBuffers(1, &m_ID);
	}
	
	void OpenGLVertexBuffer::Bind() const
	{
		FE_PROFILER_FUNC();
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		FE_PROFILER_FUNC();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		FE_CORE_ASSERT(size <= m_Size, "Too much data for this VertexBuffer!");
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	uint32_t OpenGLVertexBuffer::GetSize() const
	{
		return m_Size;
	}

	void OpenGLVertexBuffer::SetLayout(const BufferData::Layout& layout)
	{
		FE_PROFILER_FUNC();
		m_Layout = layout;
		m_LayoutSet = true;
	}

	const BufferData::Layout& OpenGLVertexBuffer::GetLayout() const
	{
		FE_PROFILER_FUNC();
		FE_CORE_ASSERT(m_LayoutSet, "Vertex Buffer has no layout!");

		return m_Layout;
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		FE_PROFILER_FUNC();
		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		FE_PROFILER_FUNC();
		glDeleteBuffers(1, &m_ID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		FE_PROFILER_FUNC();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		FE_PROFILER_FUNC();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	uint32_t OpenGLIndexBuffer::GetCount() const
	{
		return m_Count;
	}
	
}