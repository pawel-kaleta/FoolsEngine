#pragma once

#include "StaticBuffer.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"

#include <glad/glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct MeshBindingsBase
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

	struct MeshBindings_OpenGL final : MeshBindingsBase
	{
		GLuint OpenGLID;

		virtual void Create() override;
		virtual void Delete() override;

		virtual void BindVertexBuffer(const StaticBufferBase& buffer, U32 offset) override;
		virtual void BindIndexBuffer(const StaticBufferBase& buffer, U32 offset, U32 indexCount) override;
	};

	//struct MeshBindings_Vulkan final : MeshBindingsBase
	//{
	//	virtual void Create() override {};
	//	virtual void Delete() override {};
	//
	//	virtual void BindVertexBuffer(const StaticBufferBase& buffer, U32 offset) override {};
	//	virtual void BindIndexBuffer(const StaticBufferBase& buffer, U32 offset, U32 indexCount) override {};
	//};
}