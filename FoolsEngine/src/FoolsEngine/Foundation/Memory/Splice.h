#pragma once

#include "DataTypes.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"

#include <cstring>

namespace fe
{
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

		T& At(UInt i)
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


	template <typename T>
	struct Splice
	{
		T* Elements;
		UInt Count;

		void Init()
		{
			Elements = nullptr;
			Count = 0;
		}

		const	T* begin()	const	{ return Elements; }
				T* begin()			{ return Elements; }
		const	T* end()	const	{ return Elements + Count; }
				T* end()			{ return Elements + Count; }

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Elements[i];
		}

		T& At(UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Elements[i];
		}
	};
}