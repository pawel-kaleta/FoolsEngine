#pragma once

#include "DataTypes.h"
#include "FoolsEngine/Foundation/Debug/Asserts.h"

#include <cstring>

namespace fe
{
	template <typename T>
	struct Splice
	{
		T* Elements = nullptr;
		UInt Count = 0;

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
	};

	template <typename T, UInt N>
	struct Array
	{
		T Elements[N];

		inline constexpr static UInt Count = N;

		T* Begin() const { return Elements; }
		T* End() const { return Elements + Count; }

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

		T* Begin() const { return Buffer.Elements; }
		T* End() const { return Buffer.Elements + Count; }
		T* BufferEnd() const { return Buffer.Elements + Buffer.Count; }
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

		T* Begin() const { return Buffer.Elements; }
		T* End() const { return Buffer.Elements + Count; }
		T* BufferEnd() const { return Buffer.Elements + Buffer.Count; }
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