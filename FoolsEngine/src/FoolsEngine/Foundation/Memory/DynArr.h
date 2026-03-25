#pragma once

#include "DataTypes.h"
#include "Splice.h"
#include "Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Utils/Context.h"

namespace fe
{
	template <typename T>
	class DynArr
	{
	public:
		Splice<T> Buffer;
		UInt Count = 0;

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
				Context::Allocators::Default.Deallocate(Buffer);
			}
			Buffer.Elements = nullptr;
			Buffer.Count = 0;
			Count = 0;
		}

		void Append(const T* element)
		{
			if (Count == Buffer.Count)
				DefaultResizeAndRelocate();

			Buffer[Count] = *element;
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
			Count--;
			return Buffer.Elements[Count];
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity < Buffer.Count, "Attempt to reserve DynArr capacity to no more then it already have!");
			RelocateToNewCapacity(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity < Buffer.Count, "Attempt to reserve DynArr capacity to no more then it already have!");

			bool any_capacity = Buffer.Count;
			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + (UInt)4 * !any_capacity;

			bool default_better = new_capacity > capacity;
			new_capacity = new_capacity * default_better + capacity * !default_better;

			RelocateToNewCapacity(new_capacity);
		}

	private:
		void DefaultResizeAndRelocate()
		{
			bool any_capacity = Buffer.Count;
			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5

			new_capacity = new_capacity * any_capacity + (UInt)4 * !any_capacity;

			RelocateToNewCapacity(new_capacity);
		}

		void RelocateToNewCapacity(UInt newCapacity)
		{
			auto& alloc = Context::Allocators::Default;
			Splice<T> new_buffer = alloc.Allocate<T>(newCapacity);

			if (Buffer.Elements)
			{
				std::memcpy(new_buffer.Elements, Buffer.Elements, Buffer.Count * sizeof(T));
				alloc.Deallocate(Buffer);
			}

			Buffer = new_buffer;
		}
	};
}