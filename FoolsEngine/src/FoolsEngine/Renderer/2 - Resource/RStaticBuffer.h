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
		virtual void Delete() = 0;
		virtual bool IsStatic() = 0;
	};

	struct RMemReg
	{
		RBuffer* Buffer;
		U32 Offset;
		U32 Size;
	};

	struct RStaticBuffer : public RBuffer
	{
		virtual bool IsStatic() final override { return true; }

		// from RBuffer
		//virtual void Create() = 0;
		//virtual void Delete() = 0;

		virtual void Replace(Splice<Byte> data = Splice<Byte>()) = 0;
		virtual void Update(UInt targetOffset, Splice<Byte> data) = 0;
	};

	struct RGenericBuffer : public RBuffer
	{
		virtual bool IsStatic() final override { return true; }

		// from RBuffer
		//virtual void Create() = 0;
		//virtual void Delete() = 0;

		virtual void Replace(Splice<Byte> data = Splice<Byte>()) = 0;
		virtual void Update(UInt targetOffset, Splice<Byte> data) = 0;
	};

	struct RStaticBuffer_OpenGL final : public RStaticBuffer
	{
		GLuint OpenGLID;

		virtual void Create() override;
		virtual void Delete() override;

		virtual void Replace(Splice<Byte> data = Splice<Byte>()) override;
		virtual void Update(UInt targetOffset, Splice<Byte> data) override;
	};


}