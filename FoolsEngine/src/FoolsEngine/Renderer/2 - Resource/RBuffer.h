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
		virtual void Delete() override;
	};

	class RAllocator
	{
	public:
		virtual RMemReg AllocateRaw(UInt bytes) = 0;
		virtual RMemReg AllocateRaw(UInt bytes, UInt alignment) = 0;
		virtual void DeallocateRaw(RMemReg memReg) = 0;

		template <UInt Size, UInt Alignment>
		RMemReg AllocateRaw() { return AllocateRaw(Size, Alignment); }

		template <UInt Alignment>
		RMemReg AllocateRaw(UInt bytes) { return AllocateRaw(bytes, Alignment); }

		template <UInt Size>
		void DeallocateRaw(RMemReg memReg) { DeallocateRaw(memReg); } // assert Size == memReg.Size
	};
}