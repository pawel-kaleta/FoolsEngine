#pragma once

#include "DataTypes.h"
#include "Allocator.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"

namespace fe
{
	template <typename T>
	struct Splice
	{
		T* Elements = nullptr;
		UInt Count = 0;

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Elements[i];
		}
	};

	template <typename T>
	class Array : public Splice<T>
	{
	public:
		UInt Capacity = 0;

		void InitArray(T* elements, UInt capacity)
		{
			Splice<T>::Elements = elements;
			Capacity = capacity;
			Splice<T>::Count = 0;
		}

		void InitArray(MemReg& memReg)
		{
			Splice<T>::Elements = (T*)memReg.Data;
			Capacity = memReg.Size / sizeof(T);
			Splice<T>::Count = 0;
		}

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Capacity, "Out of Splice bound!");
			return Splice<T>::Elements[i];
		}

		void Append(const T& element)
		{
			FE_CORE_ASSERT(Splice<T>::Count < Capacity, "Out of Array bounds!");
			Splice<T>::Elements[Splice<T>::Count] = element;
			++Splice<T>::Count;
			return;
		}

		T& PushBack()
		{
			FE_CORE_ASSERT(Splice<T>::Count < Capacity, "Out of Array bounds!");
			T& result = Splice<T>::Elements[Splice<T>::Count];
			++Splice<T>::Count;
			return result;
		}

		T PopBack()
		{
			FE_CORE_ASSERT(Splice<T>::Count, "Array is empty!");
			T& result = Splice::Elements[Splice<T>::Count];
			Splice<T>::Count--;
			return result;
		}
	};

	// dont instantiate, use DynArrStat instead
	template <typename T>
	class DynArr : public Array<T>
	{
	public:
		Allocator* Alloc = nullptr;

		void Release()
		{
			if (Array<T>::Elements)
			{
				MemReg to_dealloc;
				to_dealloc.Data = (Byte*)Array<T>::Elements;
				to_dealloc.Size = Array<T>::Capacity * sizeof(T);
				Alloc->Deallocate(to_dealloc);
			}
			Array<T>::Capacity = 0;
			Array<T>::Count = 0;
			Array<T>::Elements = nullptr;
		}

		void Deinit()
		{
			Release();
			Alloc = nullptr;
		}

		void Append(const T& element)
		{
			if (Array<T>::Count == Array<T>::Capacity)
				DefaultResizeAndRelocate();

			Array<T>::Elements[Array<T>::Count] = element;
			++Array<T>::Count;
			return;
		}

		T& PushBack()
		{
			if (Array<T>::Count == Array<T>::Capacity)
				DefaultResizeAndRelocate();

			T& result = Array<T>::Elements[Array<T>::Count];
			++Array<T>::Count;
			return result;
		}

		void DefaultResizeAndRelocate()
		{
			bool any_capacity = Array<T>::Capacity;
			UInt new_capacity = Array<T>::Capacity + (Array<T>::Capacity >> 1); // *1.5
			
			new_capacity = new_capacity * any_capacity + (UInt)4 * !any_capacity;

			RelocateToNewCapacity(new_capacity);
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= Array<T>::Capacity, "Attempt to reserve DynArr capacity to no more then it already have!");
			RelocateToNewCapacity(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= Array<T>::Capacity, "Attempt to reserve DynArr capacity to no more then it already have!");

			bool any_capacity = Array<T>::Capacity;
			UInt new_capacity = Array<T>::Capacity + (Array<T>::Capacity >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + (UInt)4 * !any_capacity;

			bool default_better = new_capacity > capacity;
			new_capacity = new_capacity * default_better + capacity * !default_better;

			RelocateToNewCapacity(new_capacity);
		}
	private:
		void RelocateToNewCapacity(UInt newCapacity)
		{
			MemReg new_elements = Alloc->Allocate<T>(newCapacity);

			if (Array<T>::Elements)
			{
				MemReg to_dealloc;
				to_dealloc.Data = (Byte*)Array<T>::Elements;
				to_dealloc.Size = Array<T>::Capacity * sizeof(T);
				std::memcpy(new_elements.Data, Array<T>::Elements, to_dealloc.Size);
				Alloc->Deallocate(to_dealloc);
			}

			Array<T>::Elements = new_elements.Data;
			Array<T>::Capacity = newCapacity;
		}
	};

	template <typename T, class tAlloc>
	class DynArrAlloc final : public DynArr<T>
	{
	public:
		DynArrAlloc() { };
		DynArrAlloc(tAlloc& alloc) { DynArr<T>::Alloc = &alloc; }
		~DynArrAlloc() { Release(); }

		void InitDynArrAlloc(tAlloc* allocator) { DynArr<T>::Alloc = allocator; }

		void Release()
		{
			if (DynArr<T>::Elements)
			{
				MemReg to_dealloc;
				to_dealloc.Data = (Byte*)Array<T>::Elements;
				to_dealloc.Size = DynArr<T>::Capacity * sizeof(T);
				((tAlloc*)DynArr<T>::Alloc)->Deallocate(to_dealloc);
			}
			DynArr<T>::Capacity = 0;
			DynArr<T>::Count = 0;
			DynArr<T>::Elements = nullptr;
		}

		void Deinit()
		{
			Release();
			DynArr<T>::Alloc = nullptr;
		}

		void Append(const T& element)
		{
			if (DynArr<T>::Count == DynArr<T>::Capacity)
				DefaultResizeAndRelocate();

			DynArr<T>::Elements[DynArr<T>::Count] = element;
			++DynArr<T>::Count;
			return;
		}

		T& PushBack()
		{
			if (DynArr<T>::Count == DynArr<T>::Capacity)
				DefaultResizeAndRelocate();

			T& result = DynArr<T>::Elements[DynArr<T>::Count];
			++DynArr<T>::Count;
			return result;
		}

		void DefaultResizeAndRelocate()
		{
			bool any_capacity = DynArr<T>::Capacity;
			UInt new_capacity = DynArr<T>::Capacity + (DynArr<T>::Capacity >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + (UInt)4 * !any_capacity;

			RelocateToNewCapacity(new_capacity);
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= DynArr<T>::Capacity, "Attempt to reserve DynArr capacity to no more then it already have!");
			RelocateToNewCapacity(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= DynArr<T>::Capacity, "Attempt to reserve DynArr capacity to no more then it already have!");

			bool any_capacity = DynArr<T>::Capacity;
			UInt new_capacity = DynArr<T>::Capacity + (DynArr<T>::Capacity >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + (UInt)4 * !any_capacity;

			bool default_better = new_capacity > capacity;
			new_capacity = new_capacity * default_better + capacity * !default_better;

			RelocateToNewCapacity(new_capacity);
		}

	private:
		void RelocateToNewCapacity(UInt newCapacity)
		{
			MemReg new_elements = ((tAlloc*)DynArr<T>::Alloc)->Allocate<T>(newCapacity);

			if (DynArr<T>::Elements)
			{
				MemReg to_dealloc;
				to_dealloc.Data = (Byte*)DynArr<T>::Elements;
				to_dealloc.Size = DynArr<T>::Capacity * sizeof(T);
				std::memcpy(new_elements.Data, DynArr<T>::Elements, to_dealloc.Size);
				((tAlloc*)DynArr<T>::Alloc)->Deallocate(to_dealloc);
			}

			DynArr<T>::Elements = new_elements.Data;
			DynArr<T>::Capacity = newCapacity;
		}
	};

	template <typename T, class alloc>
	inline DynArrAlloc<T, alloc> MakeDynArr(alloc& al) { return DynArrAlloc<T, alloc>(al); }
}