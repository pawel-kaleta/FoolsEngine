#pragma once

#include "DataTypes.h"
#include "Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Utils/Context.h"

namespace fe
{
	struct CString
	{
		const char* Data = nullptr;
		UInt LengthWithNull = 0;

		void FromConstCharPtr(const char* string, UInt lengthWithNull)
		{
			Data = string;
			LengthWithNull = lengthWithNull;
		}

		void FromConstCharPtr(const char* string)
		{
			Data = string;

			auto current = string;
			UInt size = 0;

			while (*current != '\0')
			{
				size++;
				current++;
			}

			LengthWithNull = size + 1;
		}
	};

	struct String
	{
		const char8_t* Data = nullptr;
		UInt Length = 0;

		template <class tnAllocator>
		CString GetCString(tnAllocator* alloc) const
		{
			Splice<char> mem_reg = alloc->Allocate<char>(Length + 1);
			std::memcpy(mem_reg.Elements, Data, Length);
			mem_reg.Elements[Length] = '\0';
			return *(CString*) & mem_reg;
		}

		void FromConstCharPtr(const char* string, UInt lengthWithNull)
		{
			Data = (const char8_t*)string;
			Length = lengthWithNull - 1;
		}

		void FromCString(CString* string)
		{
			Data = (const char8_t*)string->Data;
			Length = string->LengthWithNull - 1;
		}

		void FromSplice(Splice<char8_t> splice) { *this = *(String*)&splice; }
		void FromMemReg(Splice<Byte> splice)	{ *this = *(String*)&splice; }
	};

	inline bool CompareStringsEqual(String a, String b)
	{
		if (a.Length != b.Length)
			return false;

		return ! std::memcmp(a.Data, b.Data, a.Length);
	}

	struct StringBuilder
	{
		Splice<char8_t> Buffer;
		UInt Count = 0;

		void Release()
		{
			Context::Allocators::Default->Deallocate(Buffer);
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
			std::memcpy(mem_reg.begin(), Buffer.begin(), Count);
			mem_reg.Elements[Count] = '\0';
			return *(CString*)&mem_reg;
		}

		template <class tnAllocator>
		String GetCString(tnAllocator* alloc) const
		{
			Splice<char> mem_reg = alloc->Allocate<char>(Count);
			std::memcpy(mem_reg.begin(), Buffer.begin(), Count);
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
			auto alloc = Context::Allocators::Default;
			Splice<char8_t> new_buffer = alloc->Allocate<char8_t>(newCapacity);

			if (Buffer.Elements)
			{
				std::memcpy(new_buffer.Elements, Buffer.Elements, Buffer.Count * sizeof(char8_t));
				alloc->Deallocate(Buffer);
			}

			Buffer = new_buffer;
		}
	};
}