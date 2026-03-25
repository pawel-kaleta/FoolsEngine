#pragma once

#include "Splice.h"

namespace fe
{
	// dont instantiate, use DynArrAlloc instead
	template <typename T>
	class DynArrAllocPM
	{
	public:
		Splice<T> Buffer;
		UInt Count = 0;
		TypedAlloc<Allocator> Alloc;

		T* Begin() const { return Buffer.Elements; }
		T* End() const { return Buffer.Elements + Count; }
		T* BufferEnd() const { return Buffer.Elements + Buffer.Count; }
		bool IsFull() const { return Count == Buffer.Count; }

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		const T& operator[](UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		void Release()
		{
			if (Buffer.Elements)
			{
				Alloc.Deallocate(Buffer);
			}
			Count = 0;
			Buffer.Count = 0;
			Buffer.Elements = nullptr;
		}

		void Append(const T* element)
		{
			if (Count == Buffer.Count)
				DefaultResizeAndRelocate();

			Buffer.Elements[Count] = *element;
			Count++;
			return;
		}

		T* PushBack()
		{
			if (Count == Buffer.Count)
				DefaultResizeAndRelocate();

			T* result = Buffer.Elements + Count;

			Count++;
			return result;
		}

		T PopBack()
		{
			FE_CORE_ASSERT(Count, "DynArrAllocPM is empty!");
			Count--;
			return Buffer.Elements[Count];
		}

		void DefaultResizeAndRelocate()
		{
			bool any_capacity = Buffer.Count;
			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5

			new_capacity = new_capacity * any_capacity + (UInt)4 * !any_capacity;

			RelocateToNewCapacity(new_capacity);
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= Buffer.Count, "Attempt to reserve DynArr capacity to no more then it already have!");
			RelocateToNewCapacity(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= Buffer.Count, "Attempt to reserve DynArr capacity to no more then it already have!");

			bool any_capacity = Buffer.Count;
			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + (UInt)4 * !any_capacity;

			bool default_better = new_capacity > capacity;
			new_capacity = new_capacity * default_better + capacity * !default_better;

			RelocateToNewCapacity(new_capacity);
		}
	private:
		void RelocateToNewCapacity(UInt newCapacity)
		{
			Splice<T> new_buffer = Alloc.Allocate<T>(newCapacity);

			if (Buffer.Elements)
			{
				UInt size = Buffer.Count * sizeof(T);
				std::memcpy(new_buffer.Elements, Buffer.Elements, size);
				Alloc.Deallocate(Buffer);
			}

			Buffer = new_buffer;
		}
	};

	template <typename T, class tAlloc>
	class DynArrAlloc final : public DynArrAllocPM<T>
	{
	public:
		DynArrAlloc() = default;
		DynArrAlloc(tAlloc* alloc) : DynArrAllocPM<T>::Alloc(alloc) { }
		void Init(tAlloc* alloc) { DynArrAllocPM<T>::Alloc.m_Alloc = alloc; }

		void Release()
		{
			if (DynArrAllocPM<T>::Buffer.Elements)
			{
				m_Alloc().Deallocate(DynArrAllocPM<T>::Buffer);
			}
			DynArrAllocPM<T>::Count = 0;
			DynArrAllocPM<T>::Buffer.Count = 0;
			DynArrAllocPM<T>::Buffer.Elements = nullptr;
		}

		void Append(const T* element)
		{
			if (DynArrAllocPM<T>::Count == DynArrAllocPM<T>::Buffer.Count)
				DefaultResizeAndRelocate();

			DynArrAllocPM<T>::Buffer.Elements[DynArrAllocPM<T>::Count] = element;
			DynArrAllocPM<T>::Count++;
			return;
		}

		T* PushBack()
		{
			if (DynArrAllocPM<T>::Count == DynArrAllocPM<T>::Buffer.Count)
				DefaultResizeAndRelocate();

			T& result = DynArrAllocPM<T>::Buffer.Elements[DynArrAllocPM<T>::Count];
			DynArrAllocPM<T>::Count++;
			return result;
		}

		void DefaultResizeAndRelocate()
		{
			bool any_capacity = DynArrAllocPM<T>::Buffer.Count;
			UInt new_capacity = DynArrAllocPM<T>::Buffer.Count + (DynArrAllocPM<T>::Buffer.Count >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + (UInt)4 * !any_capacity;

			RelocateToNewCapacity(new_capacity);
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= DynArrAllocPM<T>::Buffer.Count, "Attempt to reserve DynArr capacity to no more then it already have!");
			RelocateToNewCapacity(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= DynArrAllocPM<T>::Buffer.Count, "Attempt to reserve DynArr capacity to no more then it already have!");

			bool any_capacity = DynArrAllocPM<T>::Buffer.Count;
			UInt new_capacity = DynArrAllocPM<T>::Buffer.Count + (DynArrAllocPM<T>::Buffer.Count >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + (UInt)4 * !any_capacity;

			bool default_better = new_capacity > capacity;
			new_capacity = new_capacity * default_better + capacity * !default_better;

			RelocateToNewCapacity(new_capacity);
		}

	private:
		TypedAlloc<tAlloc>& m_Alloc() { return *((TypedAlloc<tAlloc>*) & DynArrAllocPM<T>::Alloc); }

		void RelocateToNewCapacity(UInt newCapacity)
		{
			Splice<T> new_elements = m_Alloc().Allocate<T>(newCapacity);

			if (DynArrAllocPM<T>::Elements)
			{
				UInt size = DynArrAllocPM<T>::Buffer.Count * sizeof(T);
				std::memcpy(new_elements.Elements, DynArrAllocPM<T>::Buffer.Elements, size);
				m_Alloc().Deallocate(DynArrAllocPM<T>::Buffer);
			}

			DynArrAllocPM<T>::Buffer = new_elements;
		}
	};

	template <typename T, class alloc>
	inline DynArrAlloc<T, alloc> MakeDynArr(alloc& al) { return DynArrAlloc<T, alloc>(al); }
}