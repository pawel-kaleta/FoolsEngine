#pragma once

#include "Allocator.h"

namespace fe
{
	class MallocAlloc : public Allocator
	{
	public:
		virtual Splice<Byte> AllocateRaw(UInt bytes) override final
		{
			Splice<Byte> result;
			result.Elements = (Byte*) operator new (bytes);
			result.Count = bytes;

			return result;
		}
		virtual Splice<Byte> AllocateRaw(UInt bytes, UInt alignment) override final
		{
			//FE_CORE_ASSERT(alignment > 8, "MallocAllocator does not align to anything more then 8");
			//return AllocateRaw(bytes); //we dont allign, because delete() takes alignment and we dont require it in interface

			Splice<Byte> result;
			result.Elements = (Byte*) operator new (bytes, (std::align_val_t)alignment);
			result.Count = bytes;

			return result;
		}
		virtual void DeallocateRaw(Splice<Byte> memReg) override final { operator delete (memReg.Elements); }

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* AllocateRaw() { return AllocateRaw(Size, Alignment); }

		template <UInt Alignment>
		Splice<Byte> AllocateRaw(UInt bytes) { return AllocateRaw(bytes, Alignment); }

		template <UInt Size>
		void DeallocateRaw(Byte* ptr) { operator delete (ptr); }
	};
}