#pragma once

#include "FoolsEngine/Foundation/DataTypes.h"
#include "FoolsEngine/Foundation/Allocators/Allocator.h"
#include "Splice.h"
#include "String.h"

namespace fe
{
	namespace Context
	{
		namespace Alloc
		{
			extern PMAlloc* Default;
			extern PMAlloc* Output;
		}
	}

	template <typename tAlloc>
	CString GetCString(String string, tAlloc* alloc = Context::Alloc::Output)
	{
		CString output;
		output.Data = alloc->Allocate<char>(string.Count + 1).Elements;
		output.CountWithNull = string.Count + 1;
		std::memcpy(output.Data, string.Data, string.Count);
		output.Data[string.Count] = '\0';
		return output;
	}

	template <typename tAlloc>
	String CopyString(String string, tAlloc* alloc = Context::Alloc::Output)
	{
		String output;
		output.Data = alloc->Allocate<char8_t>(string.Count).Elements;
		output.Count = string.Count;
		std::memcpy(output.Data, string.Data, string.Count);
		return output;
	}

	template <typename tAlloc>
	String MakeString(UInt charCount, tAlloc* alloc = Context::Alloc::Output)
	{
		String output;
		output.Data = alloc->Allocate<char8_t>(charCount).Elements;
		output.Count = charCount;
		return output;
	}

	template <typename tAlloc>
	CString MakeCString(UInt charCountWithNull, tAlloc* alloc = Context::Alloc::Output)
	{
		CString output;
		output.Data = alloc->Allocate<char>(charCountWithNull).Elements;
		output.CountWithNull = charCountWithNull;
		output.Data[charCountWithNull] = '\0';
		return output;
	}

	struct StringBuilder
	{
		PMAlloc* Alloc;
		UInt Count = 0;
		Splice<char8_t> Buffer;

		FE_TYPE_SETUP(StringBuilder)

		const	char8_t* begin()	const	{ return Buffer.Elements; }
				char8_t* begin()			{ return Buffer.Elements; }
		const	char8_t* end()		const	{ return Buffer.Elements + Count; }
				char8_t* end()				{ return Buffer.Elements + Count; }

		void Init(Allocator* allocator = Context::Alloc::Default)
		{
			Alloc = allocator->Cast<PMAlloc>().P();
			Count = 0;
			Buffer.Init();
		}

		void Release()
		{
			Alloc->Deallocate(Buffer);
			Count = 0;
			Buffer.Init();
		}

		void Clear()
		{
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

		template <typename tAlloc>
		String GetString(tAlloc* allocator = Context::Alloc::Output)
		{
			String output;
			output.Data = allocator->Cast<TypedAlloc<tAlloc>>().Allocate<char8_t>(Count).Elements;
			output.Count = Count;
			std::memcpy(output.Data, Buffer.begin(), Count);
			return output;
		}

		String GetString()
		{
			String result;
			result.Data = Buffer.Elements;
			result.Count = Count;
			return result;
		}

		template <typename tAlloc>
		CString GetCString(tAlloc* allocator = Context::Alloc::Output)
		{
			CString output;
			output.Data = allocator->Cast<TypedAlloc<tAlloc>>().Allocate<char>(Count + 1).Elements;
			output.CountWithNull = Count + 1;
			std::memcpy(output.Data, Buffer.begin(), Count);
			output.Data[Count] = '\0';
			return output;
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity > Buffer.Count, "StringBuilder allready bigger");

			if (!Buffer.Elements)
			{
				Buffer.Elements = Alloc->Allocate<char8_t>(capacity).Elements;
				Buffer.Count = capacity;
				return;
			}

			AllocateAndMove(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity > Buffer.Count, "StringBuilder allready bigger");

			if (!Buffer.Elements)
			{
				Buffer.Elements = Alloc->Allocate<char8_t>(capacity).Elements;
				Buffer.Count = capacity;
				return;
			}

			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5

			bool default_better = capacity < new_capacity;
			new_capacity = new_capacity * default_better + capacity * !default_better;

			AllocateAndMove(new_capacity);
		}

		void ExpandDefault()
		{
			if (!Buffer.Elements)
			{
				Buffer.Elements = Alloc->Allocate<char8_t>(2).Elements;
				Buffer.Count = 2;
				return;
			}

			UInt new_capacity = Buffer.Count + (Buffer.Count >> 1); // *1.5

			AllocateAndMove(new_capacity);
		}

		void AllocateAndMove(UInt capacity)
		{
			Splice<char8_t> move_splice = Buffer;
			move_splice.Count = Count;

			Splice<char8_t> new_buffer = Alloc->Reallocate<char8_t>(Buffer, move_splice, capacity);

			FE_CORE_ASSERT(new_buffer.Elements && new_buffer.Count, "Failed to reallocate StringBuilder");

			Buffer = new_buffer;
		}
	};

	template <typename tnAlloc>
	struct StringBuilderSA : public StringBuilder
	{
		FE_TYPE_SETUP(StringBuilderSA<tnAlloc>)

		void Release()
		{
			StringBuilder::Alloc->Cast<TypedAlloc<tnAlloc>>().Deallocate(Buffer);
			StringBuilder::Count = 0;
			StringBuilder::Buffer.Init();
		}

		void Append(String string)
		{
			UInt required_capacity = StringBuilder::Count + string.Count;
			if (StringBuilder::Buffer.Count < required_capacity)
				ReserveAtLeast(required_capacity);

			std::memcpy(&StringBuilder::Buffer[StringBuilder::Count], string.Data, string.Count);
			StringBuilder::Count += string.Count;
		}

		void Append(const char* ptr, UInt lengthWithNull)
		{
			UInt append_count = lengthWithNull - 1;
			UInt required_capacity = StringBuilder::Count + append_count;
			if (Buffer.Count < required_capacity)
				ReserveAtLeast(required_capacity);

			std::memcpy(&StringBuilder::Buffer[StringBuilder::Count], ptr, append_count);
			StringBuilder::Count += append_count;
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

			if (StringBuilder::Buffer.Count < StringBuilder::Count + size)
				ReserveAtLeast(StringBuilder::Count + size);

			std::memcpy(&StringBuilder::Buffer[StringBuilder::Count], ptr, size);
			StringBuilder::Count += size;
		}

		void ReserveExact(UInt capacity)
		{
			FE_CORE_ASSERT(capacity > StringBuilder::Buffer.Count, "StringBuilder allready bigger");

			if (!StringBuilder::Buffer.Elements)
			{
				StringBuilder::Buffer = StringBuilder::Alloc->Cast<TypedAlloc<tnAlloc>>().Allocate<char8_t>(capacity);
				return;
			}

			AllocateAndMove(capacity);
		}

		void ReserveAtLeast(UInt capacity)
		{
			FE_CORE_ASSERT(capacity > StringBuilder::Buffer.Count, "StringBuilder allready bigger");

			if (!StringBuilder::Buffer.Elements)
			{
				StringBuilder::Buffer = StringBuilder::Alloc->Cast<TypedAlloc<tnAlloc>>().Allocate<char8_t>(capacity);
				return;
			}

			UInt new_capacity = StringBuilder::Buffer.Count + (StringBuilder::Buffer.Count >> 1); // *1.5

			bool default_better = capacity < new_capacity;
			new_capacity = new_capacity * default_better + capacity * !default_better;

			AllocateAndMove(new_capacity);
		}

		void ExpandDefault()
		{
			if (!StringBuilder::Buffer.Elements)
			{
				StringBuilder::Buffer = StringBuilder::Alloc->Cast<TypedAlloc<tnAlloc>>().Allocate<char8_t>(2);
				return;
			}

			UInt new_capacity = StringBuilder::Buffer.Count + (StringBuilder::Buffer.Count >> 1); // *1.5

			AllocateAndMove(new_capacity);
		}

		void AllocateAndMove(UInt capacity)
		{
			Splice<char8_t> move_splice = StringBuilder::Buffer;
			move_splice.Count = StringBuilder::Count;

			Splice<char8_t> new_buffer = Alloc->Cast<TypedAlloc<tnAlloc>>().Reallocate<char8_t>(StringBuilder::Buffer, move_splice, capacity);

			FE_CORE_ASSERT(new_buffer.Elements && new_buffer.Count, "Failed to reallocate StringBuilder");

			StringBuilder::Buffer = new_buffer;
		}
	};
}