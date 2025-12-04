#pragma once

#include "FoolsEngine\Renderer\1 - Description\Buffer.h"
#include "VertexBinding.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct IndexBufferBase
	{
		Buffer::UploadType UploadType;
		Buffer::Usage Usage;
		uint32_t Count;

		virtual void Create() = 0;
		virtual void Destroy() = 0;
	};

	struct IndexBuffer_OpenGL final : IndexBufferBase
	{
		GLuint IndexBufferOpenGLID;

		virtual void Create() override;
		virtual void Destroy() override;
	};
}