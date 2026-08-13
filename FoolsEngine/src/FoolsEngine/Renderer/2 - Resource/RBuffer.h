#pragma once

#include "FoolsEngine/Renderer/1 - Description/Buffer.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"

#include <glad/glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct RBuffer
	{
		U32 Size;

		virtual void Create() = 0;
		virtual void Create(Splice<Byte> data) = 0;
		virtual void Delete() = 0;
	};

	struct RMemReg
	{
		RBuffer* Buffer;
		U32 Offset;
		U32 Size;
	};

	struct RBuffer_OpenGL final : public RBuffer
	{
		GLuint OpenGLID;

		virtual void Create() override;
		virtual void Create(Splice<Byte> data);
		virtual void Delete() override;
	};
}