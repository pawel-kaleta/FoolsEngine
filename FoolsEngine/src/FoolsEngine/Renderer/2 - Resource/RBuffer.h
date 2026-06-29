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
		virtual RMemReg Allocate(UInt bytes) = 0;
		virtual RMemReg Allocate(UInt bytes, UInt alignment) = 0;
		virtual void Deallocate(RMemReg memReg) = 0;

		template <UInt Size, UInt Alignment>
		RMemReg Allocate() { return Allocate(Size, Alignment); }

		template <UInt Alignment>
		RMemReg Allocate(UInt bytes) { return Allocate(bytes, Alignment); }

		template <UInt Size>
		void Deallocate(RMemReg memReg) { Deallocate(memReg); } // assert Size == memReg.Size
	};

	class RArenaAlloc : public RAllocator
	{
	public:
		RBuffer* Buffer;
		UInt Free;

		void Clear() { Free = 0; }

		virtual RMemReg Allocate(UInt bytes) override final
		{
			RMemReg memReg;
			memReg.Offset = (U32)AlignTo<8>((Byte*)Free);
			UInt new_free = memReg.Offset + bytes;
			if (new_free > Buffer->Size)
			{
				FE_CORE_ASSERT(false, "RArenaAllocator overflow!");
				memReg.Buffer = nullptr;
				return memReg;
			}
			memReg.Buffer = Buffer;
			memReg.Size = bytes;
			Free = new_free;
			return memReg;
		}
		virtual RMemReg Allocate(UInt bytes, UInt alignment) override final
		{
			RMemReg memReg;
			memReg.Offset = (U32)AlignTo((Byte*)Free, alignment);
			UInt new_free = memReg.Offset + bytes;
			if (new_free > Buffer->Size)
			{
				FE_CORE_ASSERT(false, "RArenaAllocator overflow!");
				memReg.Buffer = nullptr;
				return memReg;
			}
			memReg.Buffer = Buffer;
			memReg.Size = bytes;
			Free = new_free;
			return memReg;
		}
		virtual void Deallocate(RMemReg memReg) override final { FE_CORE_ASSERT(false, "RArenaAllocator does not deallocate individually"); }

		template <UInt Size, UInt Alignment>
		RMemReg Allocate()
		{
			RMemReg memReg;
			memReg.Offset = (U32)AlignTo<Alignment>((Byte*)Free);
			UInt new_free = memReg.Offset + Size;
			if (new_free > Buffer->Size)
			{
				FE_CORE_ASSERT(false, "RArenaAllocator overflow!");
				memReg.Buffer = nullptr;
				return memReg;
			}
			memReg.Buffer = Buffer;
			memReg.Size = Size;
			Free = new_free;
			return memReg;
		}

		template <UInt Alignment>
		RMemReg Allocate(UInt bytes)
		{
			RMemReg memReg;
			memReg.Offset = (U32)AlignTo<Alignment>((Byte*)Free);
			UInt new_free = memReg.Offset + bytes;
			if (new_free > Buffer->Size)
			{
				FE_CORE_ASSERT(false, "RArenaAllocator overflow!");
				memReg.Buffer = nullptr;
				return memReg;
			}
			memReg.Buffer = Buffer;
			memReg.Size = bytes;
			Free = new_free;
			return memReg;
		}

		template <UInt Size>
		void Deallocate(Byte* ptr) { FE_CORE_ASSERT(false, "RArenaAllocator does not deallocate individually"); }
	};


}