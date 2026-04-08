#pragma once

#include "DataTypes.h"
#include "Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Utils/Context.h"
#include "Splice.h"

namespace fe
{
	//struct CString
	//{
	//	const char* Data = nullptr;
	//	UInt LengthWithNull = 0;
	//
	//	bool IsValid() const { return (UInt)Data * LengthWithNull; }
	//	bool IsEmpty() const { return LengthWithNull > 1; }
	//
	//	void FromConstCharPtr(const char* string, UInt lengthWithNull)
	//	{
	//		Data = string;
	//		LengthWithNull = lengthWithNull;
	//	}
	//
	//	void FromConstCharPtr(const char* string)
	//	{
	//		Data = string;
	//
	//		auto current = string;
	//		UInt size = 0;
	//
	//		while (*current != '\0')
	//		{
	//			size++;
	//			current++;
	//		}
	//
	//		LengthWithNull = size + 1;
	//	}
	//};

	struct String
	{
		Splice<char8_t> Buffer;

		const	char8_t* Data() const	{ return Buffer.Elements; }
				char8_t* Data()			{ return Buffer.Elements; }
		const char* CData() const { return (const char*)Buffer.Elements; }

		//without null
		UInt Length() const { return Buffer.Count - 1; }

		bool IsValid() const { return Data(); }
		bool IsEmpty() const { return Length(); }

		String() = default;
		String(const char* string, UInt lengthWithNull) { FromConstCharPtr(string, lengthWithNull); }

		void FromConstCharPtr(const char* string, UInt lengthWithNull)
		{
			Buffer.Elements = (char8_t*)string;
			Buffer.Count = lengthWithNull;
		}

		void FromSplice(Splice<char8_t> splice) { *this = *(String*)&splice; }
		void FromMemReg(Splice<Byte> splice)	{ *this = *(String*)&splice; }
	};

	inline bool CompareStringsEqual(String a, String b)
	{
		if (a.Buffer.Count != b.Buffer.Count)
			return false;

		return ! std::memcmp(a.Data(), b.Data(), a.Buffer.Count);
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
			UInt Required_capacity = Count + string.Length();
			if (Buffer.Count < Required_capacity)
				ReserveAtLeast(Required_capacity);

			std::memcpy(&Buffer[Count], string.Data(), string.Length());
			Count += string.Length();
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
		String GetCString(tnAllocator* alloc) const
		{
			Splice<char8_t> mem_reg = alloc->Allocate<char8_t>(Count+1);
			std::memcpy(mem_reg.begin(), Buffer.begin(), Count);
			mem_reg.Elements[Count] = u8'\0';
			return *(String*)&mem_reg;
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