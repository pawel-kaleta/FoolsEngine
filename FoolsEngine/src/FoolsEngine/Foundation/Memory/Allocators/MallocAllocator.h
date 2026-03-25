#pragma once

#include "Allocator.h"

namespace fe
{
	class MallocAlloc : public Allocator
	{
	public:
		template <typename T, UInt Count>
		Array<T, Count>* Allocate()
		{
			auto result = Allocate<Count * sizeof(T), alignof(T)>();
			return (Array<T, Count>*)result;
		}

		template <typename T>
		T* Allocate() { return (T*)Allocate<sizeof(T), alignof(T)>(); }

		template <typename T>
		Splice<T> Allocate(UInt Count)
		{
			Splice<Byte> result = Allocate<alignof(T)>(sizeof(T) * Count);
			result.Count = Count;
			return *(Splice<T>*) & result;
		}

		template <typename T>
		void Deallocate(T* ptr) { Deallocate<sizeof(T)>((Byte*)ptr); }

		template <typename T, UInt Count>
		void Deallocate(Array<T, Count>* ptr) { Deallocate<sizeof(T)* Count>((Byte*)ptr); }

		template <typename T>
		void Deallocate(Splice<T> splice)
		{
			splice.Count *= sizeof(T);
			Deallocate(*(Splice<Byte>*) & splice);
		}
	protected:
		virtual Splice<Byte> Allocate(UInt bytes) override final
		{
			Splice<Byte> result;
			result.Elements = (Byte*) operator new (bytes);
			result.Count = bytes;

			return result;
		}
		virtual Splice<Byte> Allocate(UInt bytes, UInt alignment) override final
		{
			FE_CORE_ASSERT(alignment > 8, "MallocAllocator does not align to anything more then 8");
			return Allocate(bytes); //we dont allign, because delete() takes alignment and we dont require it in interface
		}
		virtual void Deallocate(Splice<Byte> memReg) override final { operator delete (memReg.Elements); }

		template <UInt Size, UInt Alignment>
		Array<Byte, Size>* Allocate() { return Allocate(Size, Alignment); }

		template <UInt Alignment>
		Splice<Byte> Allocate(UInt bytes) { return Allocate(bytes, Alignment); }

		template <UInt Size>
		void Deallocate(Byte* ptr) { operator delete (ptr); }
	};
}