#pragma once

#include "DataTypes.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"

#include <cstring>

namespace fe
{
	struct MemReg
	{
		Byte* Data = nullptr;
		UInt Size = 0;
	};

	struct MemRegFiller : public MemReg
	{
		Byte* Free = nullptr;

		void Init(Byte* location, UInt size)
		{
			Data = location;
			Free = location;
			Size = size;
		}

		void Init(const MemReg& memReg)
		{
			*((MemReg*)this) = memReg;
			Free = memReg.Data;
		}

		void AppendData(const Byte* data, UInt size)
		{
			FE_CORE_ASSERT(Free + size <= Data + Size, "MemReg overflow!");
			std::memcpy(Free, data, size);
			Free += size;
		}

		template <typename tn>
		void AppendObject(const tn& obj)
		{
			FE_CORE_ASSERT(Free + sizeof(obj) <= Data + Size, "MemReg overflow!");

			std::memcpy(Free, *obj, sizeof(obj));
			Free += sizeof(obj);
		}

		bool IsFull() { return Free == Data + Size; }
	};
}