#pragma once

#include "DataTypes.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"

#include <cstring>

namespace fe
{
	template <typename T, UInt N>
	struct Array;

	template <typename T>
	struct Splice
	{
		T* Elements = nullptr;
		UInt Count = 0;

		const	T* begin()	const	{ return Elements; }
				T* begin()			{ return Elements; }
		const	T* end()	const	{ return Elements + Count; }
				T* end()			{ return Elements + Count; }

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Elements[i];
		}

		const T& operator[](UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Elements[i];
		}

		template <UInt Size>
		void FromArray(Array<T, Size>* array)
		{
			Elements = array->Elements;
			Count = Size;
		}
	};

	template <typename T, UInt N>
	struct Array
	{
		T Elements[N];

		inline constexpr static UInt Count = N;

		const	T* begin()	const	{ return Elements; }
				T* begin()			{ return Elements; }
		const	T* end()	const	{ return Elements + Count; }
				T* end()			{ return Elements + Count; }

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Elements[i];
		}

		const T& operator[](UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Elements[i];
		}

		Splice<T> GetSplice()
		{
			Splice<T> result;
			result.Elements = Elements;
			result.Count = N;
			return result;
		}
	};
}