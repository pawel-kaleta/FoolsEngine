#pragma once

#include "DataTypes.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"

namespace fe
{
	template <typename tn>
	struct Splice
	{
		tn* Elements = nullptr;
		UInt Count = 0;

		tn& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Elements[i];
		}
	};

	template <typename tn>
	struct Array : public Splice<tn>
	{
		UInt Capacity = 0;

		void Init(tn* elements, UInt capacity)
		{
			Elements = elements;
			Capacity = capacity;
			Count = 0;
		}

		tn& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Capacity, "Out of Splice bound!");
			return Elements[i];
		}

		tn& Append(const tn& element)
		{
			FE_CORE_ASSERT(Count < Capacity, "Out of Array bounds!");
			Elements[Count] = element;
			tn& result = Elements[Count];
			++Count;
			return result;
		}

		tn& PushBack()
		{
			FE_CORE_ASSERT(Count < Capacity, "Out of Array bounds!");
			tn& result = Elements[Count];
			++Count;
			return result;
		}

		void PopBack()
		{
			FE_CORE_ASSERT(Count, "Array is empty!");
			Count--;
		}
	};
}