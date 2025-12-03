#pragma once

#include "FoolsEngine\Renderer\1 - Description\Buffer.h"
#include "VertexBinding.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct IndexBufferBase
	{
		Buffer::UploadType m_UploadType;
		Buffer::Usage m_Usage;
		uint32_t m_Count;

		virtual void Create(uint32_t count, const uint32_t* indices) = 0;
	};

	struct IndexBuffer_OpenGL final : IndexBufferBase
	{
		GLuint m_IndexBufferID;

		virtual void Create(uint32_t count, const uint32_t* indices) override;

		
	};
}