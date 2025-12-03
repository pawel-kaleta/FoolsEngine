#include "FE_pch.h"

#include "FoolsEngine\Renderer\2 - Resource\IndexBuffer.h"

namespace fe::Resource
{
	using namespace Description;

	void IndexBuffer_OpenGL::Create(uint32_t count, const uint32_t* indices)
	{
		glCreateBuffers(1, &m_IndexBufferID);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferID);

		FE_CORE_ASSERT(m_Usage == Buffer::Usage::Draw, "Index Buffer is only for Drawing.");

		switch (m_UploadType)
		{
		case Buffer::UploadType::Static:	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW); break;
		case Buffer::UploadType::Dynamic:	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_DYNAMIC_DRAW); break;
		case Buffer::UploadType::Stream:	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STREAM_DRAW); break;
		default:
			FE_LOG_CORE_ERROR("Unrecognized IndexBuffer UploadType, defaulting to GL_DYNAMIC_DRAW");
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_DYNAMIC_DRAW);
		}

	}
}