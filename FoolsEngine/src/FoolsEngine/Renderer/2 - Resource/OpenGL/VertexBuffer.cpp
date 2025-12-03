#include "FE_pch.h"

#include "FoolsEngine\Renderer\2 - Resource\VertexBuffer.h"

namespace fe::Resource
{
	using namespace Description;

	void VertexBuffer_OpenGL::Create(const float* vertices)
	{
		FE_PROFILER_FUNC();

		glCreateBuffers(1, &m_VertexBufferID);
		
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferID);

		FE_CORE_ASSERT(m_Usage == Buffer::Usage::Draw, "Vertex Buffer is only for Drawing.");

		switch (m_UploadType)
		{
		case Buffer::UploadType::Static:	glBufferData(GL_ARRAY_BUFFER, m_Size, vertices, GL_STATIC_DRAW); break;
		case Buffer::UploadType::Dynamic:	glBufferData(GL_ARRAY_BUFFER, m_Size, vertices, GL_DYNAMIC_DRAW); break;
		case Buffer::UploadType::Stream:	glBufferData(GL_ARRAY_BUFFER, m_Size, vertices, GL_STREAM_DRAW); break;
		default:
			FE_LOG_CORE_ERROR("Unrecognized VertexBuffer UploadType, defaulting to GL_DYNAMIC_DRAW");
			glBufferData(GL_ARRAY_BUFFER, m_Size, vertices, GL_DYNAMIC_DRAW);
		}
	}

	void VertexBuffer_OpenGL::Delete()
	{
		FE_PROFILER_FUNC();

		glDeleteBuffers(1, &m_VertexBufferID);
	}
}