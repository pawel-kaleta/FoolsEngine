#pragma once

#include "RStaticBuffer.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Foundation/Memory/DataTypes.h"

#include <glad/glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct RMeshBindings
	{
		struct AttributeArray
		{
			RStaticBuffer* VertexBuffer;
			U32 LayoutID;
			U32 VertexOffset;
			Splice<U08> AttributeBindingLocations;
		};

		RStaticBuffer* IndexBuffer;
		U32 IndexOffset;
		U32 IndexCount;

		Splice<AttributeArray> AttributeArrays;

		virtual void Create() = 0;
		virtual void Delete() = 0;

		virtual void BindVertexBuffer(const RStaticBuffer& buffer, UInt offset) = 0;
		virtual void BindIndexBuffer(const RStaticBuffer& buffer, UInt offset, UInt indexCount) = 0;
	};

	struct RMeshBindings_OpenGL final : public RMeshBindings
	{
		GLuint OpenGLID;

		virtual void Create() override;
		virtual void Delete() override;

		virtual void BindVertexBuffer(const RStaticBuffer& buffer, UInt offset) override;
		virtual void BindIndexBuffer(const RStaticBuffer& buffer, UInt offset, UInt indexCount) override;
	};

}