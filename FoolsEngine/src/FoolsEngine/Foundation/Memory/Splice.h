#pragma once

#include "DataTypes.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"
#include "FoolsEngine/Foundation/Utils/TypeSetup.h"

namespace fe
{
	template <typename T>
	struct Splice;

	template <typename T, UInt N>
	struct Array
	{
		T Elements[N];

		FE_TYPE_SETUP(Array<T, N>);

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

		const T& operator[](UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Elements[i];
		}

		const T& At(UInt i) const
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

		FE_TYPE_SETUP(Splice<T>);

		Splice<T> Init()
		{
			Elements = nullptr;
			Count = 0;
			return *this;
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

		const T& operator[](UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Elements[i];
		}

		const T& At(UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Elements[i];
		}
	};
}