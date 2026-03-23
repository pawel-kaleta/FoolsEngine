#pragma once

#include "DataTypes.h"
#include "Allocator.h"
#include "Array.h"

#include "FoolsEngine/Foundation/Utils/Core.h"

namespace fe
{
	struct CString
	{
		char* Data = nullptr;
		UInt LengthWithNull = 0;
	};

	struct String
	{
		char8_t* Data = nullptr;
		UInt Length = 0;

		template <class tnAllocator>
		CString GetCString(tnAllocator& alloc) const
		{
			MemReg memReg = alloc.Allocate(Length + 1, 1);
			std::memcpy(memReg.Data, Data, Length);
			*(char*)&memReg.Data[Length] = '\0';
			return *(CString*)&memReg;
		}

		void FromConstCharPtr(const char* string, UInt lengthWithNull)
		{
			Data = (char8_t*)string;
			Length = lengthWithNull - 1;
		}

		void FromCString(CString* string)
		{
			Data = (char8_t*)string->Data;
			Length = string->LengthWithNull - 1;
		}
	};

	struct StringBuilder
	{
		DynArr<char8_t> Container;

		void Append(String string)
		{
			UInt new_count = Container.Count + string.Length;
			Container.ReserveAtLeast(new_count);
			std::memcpy(&(Container.Elements[Container.Count]), string.Data, string.Length);
			Container.Count = new_count;
		}

		template <class tnAllocator>
		CString GetCString(tnAllocator* alloc)
		{
			MemReg memReg = alloc.Allocate(Container.Count + 1, 1);
			std::memcpy(memReg.Data, Container.Elements, Container.Count);
			memReg.Data[Container.Count] = u8'\0';
			return (CString)memReg;
		}

		template <class tnAllocator>
		String GetString(tnAllocator* alloc)
		{
			MemReg memReg = alloc.Allocate(Container.Count, 1);
			std::memcpy(memReg.Data, Container.Elements, Container.Count);
			return (String)memReg;
		}
	};

	template <class tnAllocator>
	struct StringBuilderAlloc final : public StringBuilder
	{
		void Append(String string)
		{
			UInt new_count = Container.Count + string.Length;
			((DynArrAlloc<char8_t, tnAllocator>)Container).ReserveAtLeast(new_count);
			std::memcpy(&(Container.Elements[Container.Count]), string.Data, string.Length);
			Container.Count = new_count;
		}
	};
}