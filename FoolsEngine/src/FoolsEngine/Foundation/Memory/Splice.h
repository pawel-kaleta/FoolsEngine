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

	template <typename T>
	struct SpliceArena
	{
		Splice<T> Buffer;
		UInt Count = 0;

		const	T* begin() const	{ return Buffer.Elements; }
				T* begin()			{ return Buffer.Elements; }
		const	T* end() const	{ return Buffer.Elements + Count; }
				T* end()		{ return Buffer.Elements + Count; }

		bool IsFull() const { return Count == Buffer.Count; }

		void Init(Splice<T> splice)
		{
			Buffer = splice;
			Count = 0;
		}

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		const T& operator[](UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		void Append(const T* data)
		{
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");
			Buffer[Count] = *data;
			Count++;
		}

		void Append(T data)
		{
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");
			Buffer[Count] = data;
			Count++;
		}

		void Append(Splice<T> splice)
		{
			FE_CORE_ASSERT(Count + splice.Count < Buffer.Count, "Arena overflow!");
			std::memcpy(&Buffer[Count], splice.Begin(), splice.Count);
			Count += splice.Count;
		}

		T* PushBack()
		{
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");
			T* result = Buffer.Elements + Count;
			Count++;
		}

		T PopBack()
		{
			FE_CORE_ASSERT(Count, "Arena is empty!");
			Count--;
			T* result = Buffer.Elements + Count;
			return *result;
		}

		Splice<T> GetSplice()
		{
			Splice<T> result;
			result.Elements = Buffer.Elements;
			result.Count = Count;
			return result;
		}
	};

	template <typename T, UInt N>
	struct ArrayArena
	{
		UInt Count = 0;
		Array<T, N> Buffer;

		const	T* begin() const	{ return Buffer.Elements; }
				T* begin()			{ return Buffer.Elements; }
		const	T* end() const	{ return Buffer.Elements + Count; }
				T* end()		{ return Buffer.Elements + Count; }

		bool IsFull() const { return Count == Buffer.Count; }

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		const T& operator[](UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Splice bound!");
			return Buffer[i];
		}

		void Append(const T* data)
		{
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");
			Buffer[Count] = *data;
			Count++;
		}

		void Append(T data)
		{
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");
			Buffer[Count] = data;
			Count++;
		}

		T* PushBack()
		{
			FE_CORE_ASSERT(Count < Buffer.Count, "Arena overflow!");
			T* result = Buffer.Elements + Count;
			Count++;
		}

		T PopBack()
		{
			FE_CORE_ASSERT(Count, "Arena is empty!");
			Count--;
			T* result = Buffer.Elements + Count;
			return *result;
		}

		Splice<T> GetSplice()
		{
			Splice<T> result;
			result.Elements = Buffer.Elements;
			result.Count = Count;
			return result;
		}
	};
}