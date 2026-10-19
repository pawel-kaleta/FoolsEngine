#pragma once

#include "Allocator.h"

#include "FoolsEngine/Foundation/Debug/Asserts.h"

namespace fe
{
	class NullAlloc : public Allocator
	{
	public:
		virtual Splice<Byte> AllocateRaw(UInt bytes) override final { return Splice<Byte>().Init(); }
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) override final { return Splice<Byte>().Init(); }
		virtual void DeallocateRaw(Splice<Byte> memReg) override final { FE_CORE_ASSERT(!memReg.Elements, "NullAllocator should not be used for deallocation"); }

		virtual Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize, UInt alignment) override final
		{
			FE_CORE_ASSERT(!allocation.Elements, "NullAllocator should not be used for reallocation");
			return Splice<Byte>().Init();
		};

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, Splice<Byte> moveRegion, UInt newSize) { return ReallocateRaw(allocation, moveRegion, newSize, Alignment); };

		template <UInt Alignment>
		Splice<Byte> ReallocateRaw(Splice<Byte> allocation, UInt newSize) { return ReallocateRaw(allocation, allocation, newSize, Alignment); };

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* AllocateRaw() { return nullptr; }

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt bytes) { return Splice<Byte>().Init(); }

		template <UInt Size>
		void DeallocateRaw(Byte* ptr) { FE_CORE_ASSERT(!ptr, "NullAllocator should not be used for deallocation"); }
	};
}