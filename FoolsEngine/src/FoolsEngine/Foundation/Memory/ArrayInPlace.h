#pragma once

#include "Array.h"

namespace fe
{
	template <typename T, UInt count>
	struct SpliceInPlace final : public Splice<T>
	{
		// inherited
		//T* Elements = nullptr;
		//UInt Count = 0;

		T InPlace[count];

		inline constexpr UInt CountStatic() { return count; }

		void InitSpliceInPlace()
		{
			Splice<T>::Elements = InPlace;
			Splice<T>::Count = count;
		}

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < count, "Out of Splice bound!");
			return InPlace[i];
		}
	};

	template <typename T, UInt capacity>
	class ArrayInPlace final : public Array<T>
	{
	public:
		// inherited
		// T* Elements = nullptr;
		// UInt Count = 0;
		// UInt Capacity = 0;

		T InPlace[capacity];

		inline constexpr UInt CapacityStatic() { return capacity; }

		void InitArrayInPlace()
		{
			Array<T>::Elements = InPlace;
			Array<T>::Capacity = capacity;
			Array<T>::Count = 0;
		}

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < capacity, "Out of Splice bound!");
			return InPlace[i];
		}

		void Append(const T& element)
		{
			FE_CORE_ASSERT(Array<T>::Count < capacity, "Out of Array bounds!");
			InPlace[Array<T>::Count] = element;
			++Array<T>::Count;
			return;
		}

		T& PushBack()
		{
			FE_CORE_ASSERT(Array<T>::Count < capacity, "Out of Array bounds!");
			T& result = InPlace[Array<T>::Count];
			++Array<T>::Count;
			return result;
		}

		T PopBack()
		{
			FE_CORE_ASSERT(Array<T>::Count, "Array is empty!");
			T& result = InPlace[Array<T>::Count];
			Array<T>::Count--;
			return result;
		}
	};
}