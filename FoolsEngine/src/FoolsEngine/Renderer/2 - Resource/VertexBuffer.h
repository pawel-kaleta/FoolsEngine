#pragma once

#include "FoolsEngine\Renderer\1 - Description\Buffer.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct VertexBufferBase
	{
		Buffer::UploadType m_UploadType;
		Buffer::Usage m_Usage;
		uint32_t m_Size;

		virtual void Create(const float* vertices) = 0;
		virtual void Delete() = 0;
	};

	struct VertexBuffer_OpenGL final : VertexBufferBase
	{
		GLuint m_VertexBufferID;

		virtual void Create(const float* vertices) override;
		virtual void Delete() override;
	};
}