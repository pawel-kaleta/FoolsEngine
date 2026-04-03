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
		TypedAlloc<Allocator>* Alloc;

		const	T* begin() const	{ return Buffer.Elements; }
				T* begin()			{ return Buffer.Elements; }
		const	T* end() const	{ return Buffer.Elements + Count; }
				T* end()		{ return Buffer.Elements + Count; }
		
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
				Alloc->Deallocate(Buffer);
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

		void Append(Splice<T> splice)
		{
			UInt new_count = Count + splice.Count;
			if (new_count > Buffer.Count)
				ReserveAtLeast(new_count);

			std::memcpy(&Buffer[Count], splice.begin(), splice.Count);
			Count = new_count;
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

			new_capacity = any_capacity ? new_capacity : 4;

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
			new_capacity = any_capacity ? new_capacity : 4;

			bool default_better = new_capacity > capacity;
			new_capacity = default_better ? new_capacity : capacity;

			RelocateToNewCapacity(new_capacity);
		}
	private:
		void RelocateToNewCapacity(UInt newCapacity)
		{
			Splice<T> new_buffer = Alloc->Allocate<T>(newCapacity);

			if (Buffer.Elements)
			{
				UInt size = Buffer.Count * sizeof(T);
				std::memcpy(new_buffer.Elements, Buffer.Elements, size);
				Alloc->Deallocate(Buffer);
			}

			Buffer = new_buffer;
		}
	};

	template <typename T, class tAlloc>
	class DynArrAlloc final : public DynArrAllocPM<T>
	{
	public:
		DynArrAlloc() = default;
		DynArrAlloc(tAlloc* alloc) { this->Alloc = (TypedAlloc<Allocator>*)alloc; }
		void Init(tAlloc* alloc) { this->Alloc = (TypedAlloc<Allocator>*)alloc; }

		void Release()
		{
			if (this->Buffer.Elements)
			{
				GetAlloc()->Deallocate(this->Buffer);
			}
			this->Count = 0;
			this->Buffer.Count = 0;
			this->Buffer.Elements = nullptr;
		}

		void Append(const T* element)
		{
			if (this->Count == this->Buffer.Count)
				DefaultResizeAndRelocate();

			this->Buffer.Elements[this->Count] = *element;
			this->Count++;
			return;
		}

		void Append(Splice<T> splice)
		{
			UInt new_count = this->Count + splice.Count;
			if (new_count > this->Buffer.Count)
				ReserveAtLeast(new_count);

			std::memcpy(&(this->Buffer[this->Count]), splice.begin(), splice.Count);
			this->Count = new_count;
		}

		T* PushBack()
		{
			if (this->Count == this->Buffer.Count)
				DefaultResizeAndRelocate();

			T& result = this->Buffer.Elements[this->Count];
			this->Count++;
			return &result;
		}

		void DefaultResizeAndRelocate()
		{
			bool any_capacity = this->Buffer.Count;
			UInt new_capacity = this->Buffer.Count + (this->Buffer.Count >> 1); // *1.5
			new_capacity = any_capacity ? new_capacity : 4;

			RelocateToNewCapacity(new_capacity);
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= this->Buffer.Count, "Attempt to reserve DynArr capacity to no more then it already have!");
			RelocateToNewCapacity(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= this->Buffer.Count, "Attempt to reserve DynArr capacity to no more then it already have!");

			bool any_capacity = this->Buffer.Count;
			UInt new_capacity = this->Buffer.Count + (this->Buffer.Count >> 1); // *1.5
			new_capacity = any_capacity ? new_capacity : 4;

			bool default_better = new_capacity > capacity;
			new_capacity = default_better ? new_capacity : capacity;

			RelocateToNewCapacity(new_capacity);
		}

	private:
		TypedAlloc<tAlloc>* GetAlloc() { return ((TypedAlloc<tAlloc>*) this->Alloc); }

		void RelocateToNewCapacity(UInt newCapacity)
		{
			Splice<T> new_elements = GetAlloc()->Allocate<T>(newCapacity);

			if (this->Buffer.Elements)
			{
				UInt size = this->Buffer.Count * sizeof(T);
				std::memcpy(new_elements.Elements, this->Buffer.Elements, size);
				GetAlloc()->Deallocate(this->Buffer);
			}

			this->Buffer = new_elements;
		}
	};

	template <typename T, class alloc>
	inline DynArrAlloc<T, alloc> MakeDynArr(alloc& al) { return DynArrAlloc<T, alloc>(al); }
}