#pragma once

#include "StaticBuffer.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"

#include <glad/glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct VertexArrayBase
	{
		U32 LayoutID;
		U32 VertexOffset;
		U32 IndexOffset;
		U32 IndexCount;
		// ptrs to buffers with vertex data and index data?

		virtual void Create() = 0;
		virtual void Delete() = 0;

		virtual void BindVertexBuffer(const StaticBufferBase& buffer, U32 offset) = 0;
		virtual void BindIndexBuffer(const StaticBufferBase& buffer, U32 offset, U32 indexCount) = 0;
	};

	struct VertexArray_OpenGL final : VertexArrayBase
	{
		GLuint OpenGLID;

		virtual void Create() override;
		virtual void Delete() override;

		virtual void BindVertexBuffer(const StaticBufferBase& buffer, U32 offset) override;
		virtual void BindIndexBuffer(const StaticBufferBase& buffer, U32 offset, U32 indexCount) override;
	};

	//struct VertexArray_Vulkan final : VertexArrayBase
	//{
	//	virtual void Create() override {};
	//	virtual void Delete() override {};
	//
	//	virtual void BindVertexBuffer(const StaticBufferBase& buffer, U32 offset) override {};
	//	virtual void BindIndexBuffer(const StaticBufferBase& buffer, U32 offset, U32 indexCount) override {};
	//};
}