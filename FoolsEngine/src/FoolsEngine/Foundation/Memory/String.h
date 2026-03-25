#pragma once

#include "DataTypes.h"
#include "Allocator.h"
#include "DynArrAlloc.h"

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
		CString GetCString(tnAllocator* alloc) const
		{
			Splice<char> mem_reg = alloc->Allocate<char>(Length + 1);
			std::memcpy(mem_reg.Elements, Data, Length);
			mem_reg.Elements[Length] = '\0';
			return *(CString*) & mem_reg;
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

	template <class tnAlloc>
	struct StringBuilder
	{
		DynArrAlloc<String, tnAlloc> Container;

		void Init(tnAlloc* alloc) { Container.Init(alloc); }

		void Append(String string) { Container.Append(string); }

		template <class tnAllocator>
		CString GetCString(tnAllocator* alloc)
		{
			UInt size = 0;
			for (UInt i=0; i<Container.)

			Splice<char> mem_reg = alloc->Allocate<char>(Container.Count + 1);
			std::memcpy(mem_reg.Elements, Container.Buffer.Elements, Container.Count);
			mem_reg.Elements[Container.Count] = '\0';
			return *(CString*) & mem_reg;
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