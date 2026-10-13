#pragma once

#include "DataTypes.h"
#include "Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Utils/Context.h"
#include "Splice.h"

namespace fe
{
	// do we need owning string?

	struct String;

	struct CString
	{
		char* Data = nullptr;
		UInt CountWithNull = 0;

		CString() = default;
		CString(const char* string, UInt lengthWithNull) { FromConstCharPtr(string, lengthWithNull); }

		void Init()
		{
			Data = nullptr;
			CountWithNull = 0;
		}

		void FromConstCharPtr(const char* string, UInt lengthWithNull)
		{
			Data = (char*)string;
			CountWithNull = lengthWithNull;
		}

		inline String GetString();
	};

	struct String
	{
		char8_t* Data = nullptr;
		UInt Count = 0;

		String() = default;
		String(const char* string, UInt lengthWithNull) { FromConstCharPtr(string, lengthWithNull); }
		String(const char* string)
		{
			Data = (char8_t*)string;
			auto current = string;

			while (*current != '\0')
			{
				++Count;
				++current;
			}
		}

		void Init()
		{
			Data = nullptr;
			Count = 0;
		}

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

		template <typename tAlloc>
		CString GetCString(tAlloc* alloc) const
		{
			CString output;
			output.Data = alloc->Allocate<char>(Count + 1).Elements;
			output.CountWithNull = Count + 1;
			std::memcpy(output.Data, Data, Count);
			output.Data[Count + 1] = u8'\0';
			return output;
		}
	};

	inline String CString::GetString()
	{
		String result;
		result.Data = (char8_t*)Data;
		result.Count = CountWithNull - 1;
		return result;
	}

	inline bool CompareStringsEqual(String a, String b)
	{
		if (a.Count != b.Count)
			return false;

		return ! std::memcmp(a.Data, b.Data, a.Count);
	}

	struct StringBuilder
	{
		PMAlloc* Alloc;
		UInt Count = 0;
		Splice<char8_t> Buffer;

		FE_TYPE_SETUP(StringBuilder)

		const	char8_t * begin()	const	{ return Buffer.Elements; }
				char8_t * begin()			{ return Buffer.Elements; }
		const	char8_t * end()		const	{ return Buffer.Elements + Count; }
				char8_t * end()				{ return Buffer.Elements + Count; }

		void Init(Allocator* allocator = Context::Allocators::Default)
		{
			Alloc = (PMAlloc*) allocator;
			Count = 0;
			Buffer.Init();
		}

		void Release()
		{
			Alloc->Deallocate(Buffer);
			Count = 0;
			Buffer.Init();
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

		template <typename tAlloc>
		String GetString(tAlloc* allocator = Context::Allocators::Output)
		{
			String output;
			output.Data = ((TypedAlloc<tAlloc>*)allocator)->Allocate<char8_t>(Count).Elements;
			output.Count = Count;
			std::memcpy(output.Data, Buffer.begin(), Count);
			return output;
		}

		template <typename tAlloc>
		CString GetCString(tAlloc* allocator = Context::Allocators::Output)
		{
			CString output;
			output.Data = ((TypedAlloc<tAlloc>*)allocator)->Allocate<char>(Count+1).Elements;
			output.CountWithNull = Count + 1;
			std::memcpy(output.Data, Buffer.begin(), Count);
			output.Data[Count] = u8'\0';
			return output;
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity > Buffer.Count, "StringBuilder allready bigger");

			AllocateAndMove(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity > Buffer.Count, "StringBuilder allready bigger");

			bool any_capacity = Buffer.Count;
			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + 2 * !any_capacity;

			bool default_better = capacity < new_capacity;
			new_capacity = new_capacity * default_better + capacity * !default_better;

			AllocateAndMove(new_capacity);
		}

		void ExpandDefault()
		{
			bool any_capacity = Buffer.Count;
			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5
			new_capacity = new_capacity * any_capacity + 2 * !any_capacity;

			AllocateAndMove(new_capacity);
		}

		void AllocateAndMove(UInt capacity)
		{
			Splice<char8_t> new_buffer = Alloc->Allocate<char8_t>(capacity);

			if (Buffer.Elements)
			{
				std::memcpy(new_buffer.Elements, Buffer.Elements, Buffer.Count * sizeof(char8_t));
				Alloc->Deallocate(Buffer);
			}

			Buffer = new_buffer;
		}
	};
}