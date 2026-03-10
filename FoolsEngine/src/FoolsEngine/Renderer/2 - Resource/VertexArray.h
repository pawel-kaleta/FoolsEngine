#pragma once

#include "FoolsEngine\Renderer\1 - Description\Library.h"
#include "StaticBuffer.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct VertexArrayBase
	{
		uint32_t LayoutID;
		uint32_t VertexOffset;
		uint32_t IndexOffset;
		uint32_t IndexCount;
		// ptrs to buffers with vertex data and index data?

		virtual void Create() = 0;
		virtual void Delete() = 0;

		virtual void BindVertexBuffer(const StaticBufferBase& buffer, uint32_t offset) = 0;
		virtual void BindIndexBuffer(const StaticBufferBase& buffer, uint32_t offset, uint32_t indexCount) = 0;
	};

	struct VertexArray_OpenGL final : VertexArrayBase
	{
		GLuint OpenGLID;

		virtual void Create() override;
		virtual void Delete() override;

		virtual void BindVertexBuffer(const StaticBufferBase& buffer, uint32_t offset) override;
		virtual void BindIndexBuffer(const StaticBufferBase& buffer, uint32_t offset, uint32_t indexCount) override;
	};

	struct VertexArray_Vulkan final : VertexArrayBase
	{
		virtual void Create() override {};
		virtual void Delete() override {};

		virtual void BindVertexBuffer(const StaticBufferBase& buffer, uint32_t offset) override {};
		virtual void BindIndexBuffer(const StaticBufferBase& buffer, uint32_t offset, uint32_t indexCount) override {};
	};
}