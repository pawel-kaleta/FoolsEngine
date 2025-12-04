#pragma once

#include "FoolsEngine\Renderer\1 - Description\Buffer.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct VertexBufferBase
	{
		Buffer::UploadType UploadType;
		Buffer::Usage Usage;
		uint32_t Size;

		virtual void Create() = 0;
		virtual void Delete() = 0;
	};

	struct VertexBuffer_OpenGL final : VertexBufferBase
	{
		GLuint VertexBufferID;

		virtual void Create() override;
		virtual void Delete() override;
	};
}