#pragma once

#include "FoolsEngine/Renderer/1 - Description/Buffer.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"

#include <glad/glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct StaticBufferBase
	{
		Buffer::Usage Usage;
		U32 Size;

		virtual void Create() = 0;
		virtual void Delete() = 0;

		virtual void Replace(Splice<Byte> data = Splice<Byte>()) = 0;
		virtual void Update(UInt targetOffset, Splice<Byte> data) = 0;
	};

	template <GAPIType::ValueType GAPI>
	struct RStaticBuffer;

	template<>
	struct RStaticBuffer<GAPIType::OpenGL> final : public StaticBufferBase
	{
		GLuint OpenGLID;

		virtual void Create() override;
		virtual void Delete() override;

		virtual void Replace(Splice<Byte> data = Splice<Byte>()) override;
		virtual void Update(UInt targetOffset, Splice<Byte> data) override;
	};

	using RStaticBuffer_OpenGL = RStaticBuffer<GAPIType::OpenGL>;
}