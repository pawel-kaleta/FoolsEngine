#pragma once

#include "DataTypes.h"
#include "Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Utils/Context.h"
#include "Splice.h"

namespace fe
{
	struct CString
	{
		char* Data = nullptr;
		UInt CountWithNull = 0;

		CString() = default;
		CString(const char* string, UInt lengthWithNull) { FromConstCharPtr(string, lengthWithNull); }

		void FromConstCharPtr(const char* string, UInt lengthWithNull)
		{
			Data = (char*)string;
			CountWithNull = lengthWithNull;
		}
	};

	struct String
	{
		char8_t* Data = nullptr;
		UInt Count = 0;

		String() = default;
		String(const char* string, UInt lengthWithNull) { FromConstCharPtr(string, lengthWithNull); }

		void FromConstCharPtr(const char* string, UInt lengthWithNull)
		{
			Data = (char8_t*)string;
			Count = lengthWithNull-1;
		}

		template <typename T>
		void FromSplice(Splice<T> splice)
		{
			Data = (char8_t*)splice.Elements;
			Count = splice.Count * sizeof(T);
		}

		template <typename Allocator>
		CString GetCString(Allocator* alloc) const
		{
			CString output;
			output.Data = alloc->Allocate<char8_t>(Count + 1).Elements;
			output.CountWithNull = Count + 1;
			std::memcpy(output.Data, Data, Count);
			output.Data[Count + 1] = u8'\0';
			return output;
		}
	};

	inline bool CompareStringsEqual(String a, String b)
	{
		if (a.Count != b.Count)
			return false;

		return ! std::memcmp(a.Data, b.Data, a.Count);
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
			UInt required_capacity = Count + string.Count;
			if (Buffer.Count < required_capacity)
				ReserveAtLeast(required_capacity);

			std::memcpy(&Buffer[Count], string.Data, string.Count);
			Count += string.Count;
		}

		void Append(const char* ptr, UInt lengthWithNull)
		{
			UInt append_count = lengthWithNull - 1;
			UInt required_capacity = Count + append_count;
			if (Buffer.Count < required_capacity)
				ReserveAtLeast(required_capacity);

			std::memcpy(&Buffer[Count], ptr, append_count);
			Count += append_count;
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

		String GetString() const
		{
			Splice<char8_t> mem_reg = Context::Allocators::Output->Allocate<char8_t>(Count);
			std::memcpy(mem_reg.begin(), Buffer.begin(), Count);
			return String((char*)mem_reg.Elements, mem_reg.Count);
		}

		CString GetCString() const
		{
			CString output;
			output.Data = Context::Allocators::Output->Allocate<char8_t>(Count+1).Elements;
			output.CountWithNull = Count + 1;
			std::memcpy(output.Data, Buffer.begin(), Count);
			output.Data[Count + 1] = u8'\0';
			return output;
		}

		void ReserveExact(UInt capacityWithoutNull)
		{
			FE_CORE_ASSERT(capacityWithoutNull+1 > Buffer.Count, "Attempt to reserve StringBuilder capacity to no more then it already have!");
			RelocateToNewCapacity(capacityWithoutNull+1);
		}

		void ReserveAtLeast(UInt capacityWithoutNull)
		{
			FE_CORE_ASSERT(capacityWithoutNull > Buffer.Count, "Attempt to reserve StringBuilder capacity to no more then it already have!");

			bool any_capacity = Buffer.Count;
			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5
			new_capacity = any_capacity ? new_capacity : 8;

			bool default_better = new_capacity > capacityWithoutNull+1;
			new_capacity = default_better ? new_capacity : capacityWithoutNull+1;

			RelocateToNewCapacity(new_capacity);
		}
	private:
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