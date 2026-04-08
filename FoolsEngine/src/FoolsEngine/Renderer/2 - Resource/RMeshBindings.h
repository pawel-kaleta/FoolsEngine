#pragma once

#include "RStaticBuffer.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"

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

		virtual void BindVertexBuffer(const StaticBufferBase& buffer, UInt offset) = 0;
		virtual void BindIndexBuffer(const StaticBufferBase& buffer, UInt offset, UInt indexCount) = 0;
	};

	template <GAPIType::ValueType GAPI>
	struct RMeshBindings;

	template <>
	struct RMeshBindings<GAPIType::OpenGL> final : public MeshBindingsBase
	{
		GLuint OpenGLID;

		virtual void Create() override;
		virtual void Delete() override;

		virtual void BindVertexBuffer(const StaticBufferBase& buffer, UInt offset) override;
		virtual void BindIndexBuffer(const StaticBufferBase& buffer, UInt offset, UInt indexCount) override;
	};

	using RMeshBindings_OpenGL = RMeshBindings<GAPIType::OpenGL>;
}