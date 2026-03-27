#pragma once

#include "DataTypes.h"
#include "String.h"
#include "Allocators/Allocator.h"

namespace fe
{
	struct StringBuilderAllocPM
	{
		Splice<char8_t> Buffer;
		UInt Count = 0;
		TypedAlloc<Allocator>* Alloc;

		void Release()
		{
			Alloc->Deallocate(Buffer);
			Buffer.Elements = nullptr;
			Buffer.Count = 0;
			Count = 0;
		}

		void Append(String string)
		{
			if (Buffer.Count < Count + string.Length)
				ReserveAtLeast(Count + string.Length);

			std::memcpy(&Buffer[Count], string.Data, string.Length);
			Count += string.Length;
		}

		void Append(const char* ptr)
		{
			auto current = ptr;
			UInt size = 0;

			while (*current != '\0')
			{
				size++;
				current++;
			}

			if (Buffer.Count < Count + size)
				ReserveAtLeast(Count + size);

			std::memcpy(&Buffer[Count], ptr, size);
			Count += size;
		}

		template <class tnAllocator>
		CString GetCString(tnAllocator* alloc) const
		{
			Splice<char> mem_reg = alloc->Allocate<char>(Count + 1);
			std::memcpy(mem_reg.Elements, Data, Count);
			mem_reg.Elements[Count] = '\0';
			return *(CString*)&mem_reg;
		}

		template <class tnAllocator>
		String GetCString(tnAllocator* alloc) const
		{
			Splice<char> mem_reg = alloc->Allocate<char>(Count);
			std::memcpy(mem_reg.Elements, Data, Count);
			mem_reg.Elements[Count] = '\0';
			return *(CString*)&mem_reg;
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= Buffer.Count, "Attempt to reserve StringBuilder capacity to no more then it already have!");
			RelocateToNewCapacity(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= Buffer.Count, "Attempt to reserve StringBuilder capacity to no more then it already have!");

			bool any_capacity = Buffer.Count;
			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5
			new_capacity = any_capacity ? new_capacity : 8;

			bool default_better = new_capacity > capacity;
			new_capacity = default_better ? new_capacity : capacity;

			RelocateToNewCapacity(new_capacity);
		}

	private:
		void DefaultResizeAndRelocate()
		{
			bool any_capacity = Buffer.Count;
			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5

			new_capacity = any_capacity ? new_capacity : 8;

			RelocateToNewCapacity(new_capacity);
		}

		void RelocateToNewCapacity(UInt newCapacity)
		{
			Splice<char8_t> new_buffer = Alloc->Allocate<char8_t>(newCapacity);

			if (Buffer.Elements)
			{
				std::memcpy(new_buffer.Elements, Buffer.Elements, Buffer.Count * sizeof(char8_t));
				Alloc->Deallocate(Buffer);
			}

			Buffer = new_buffer;
		}
	};

	template <class tnAllocator>
	struct StringBuilderAlloc final : public StringBuilder
	{
	public:
		void Release()
		{
			GetAlloc()->Deallocate(this->Buffer);
			this->Buffer.Elements = nullptr;
			this->Buffer.Count = 0;
			this->Count = 0;
		}

		void Append(String string)
		{
			if (this->Buffer.Count < this->Count + string.Length)
				ReserveAtLeast(this->Count + string.Length);

			std::memcpy(&(this->Buffer[this->Count]), string.Data, string.Length);
			this->Count += string.Length;
		}

		void Append(const char* ptr)
		{
			auto current = ptr;
			UInt size = 0;

			while (*current != '\0')
			{
				size++;
				current++;
			}

			if (this->Buffer.Count < Count + size)
				ReserveAtLeast(Count + size);

			std::memcpy(&(this->Buffer[Count]), ptr, size);
			this->Count += size;
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= this->Buffer.Count, "Attempt to reserve StringBuilder capacity to no more then it already have!");
			RelocateToNewCapacity(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity <= this->Buffer.Count, "Attempt to reserve StringBuilder capacity to no more then it already have!");

			bool any_capacity = this->Buffer.Count;
			UInt new_capacity = this->Buffer.Count + (this->Buffer.Count >> 1); // *1.5
			new_capacity = any_capacity ? new_capacity : 8;

			bool default_better = new_capacity > capacity;
			new_capacity = default_better ? new_capacity : capacity;

			RelocateToNewCapacity(new_capacity);
		}

	private:
		TypedAlloc<tnAllocator>* GetAlloc() { return ((TypedAlloc<tnAllocator>*) this->Alloc); }

		void DefaultResizeAndRelocate()
		{
			bool any_capacity = this->Buffer.Count;
			UInt new_capacity = this->Buffer.Count + (this->Buffer.Count >> 1); // *1.5

			new_capacity = any_capacity ? new_capacity : 8;

			RelocateToNewCapacity(new_capacity);
		}

		void RelocateToNewCapacity(UInt newCapacity)
		{
			Splice<char8_t> new_buffer = GetAlloc()->Allocate<char8_t>(newCapacity);

			if (this->Buffer.Elements)
			{
				std::memcpy(new_buffer.Elements, this->Buffer.Elements, this->Buffer.Count * sizeof(char8_t));
				GetAlloc()->Deallocate(Buffer);
			}

			this->Buffer = new_buffer;
		}
	};
}