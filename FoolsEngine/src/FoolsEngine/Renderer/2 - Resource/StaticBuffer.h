#pragma once

#include "FoolsEngine\Renderer\1 - Description\Buffer.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct StaticBufferBase
	{
		Buffer::Usage Usage;
		uint32_t Size;

		virtual void Create() = 0;
		virtual void Delete() = 0;

		virtual void Upload(size_t size, const void* data = nullptr) = 0;
		virtual void Update(size_t targetOffset, size_t size, const void* data) = 0;
	};

	struct StaticBuffer_OpenGL final : StaticBufferBase
	{
		GLuint OpenGLID;

		virtual void Create() override;
		virtual void Delete() override;

		virtual void Upload(size_t size, const void* data = nullptr) override;
		virtual void Update(size_t targetOffset, size_t size, const void* data) override;
	};
}