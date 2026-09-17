#pragma once

#include "Splice.h"
#include "FoolsEngine/Foundation/Utils/Context.h"

#include <cstring>

namespace fe
{
	template <typename T>
	struct SpliceArena
	{
		Splice<T> Buffer;
		UInt Count = 0;

		const	T* begin() const { return Buffer.Elements; }
		T* begin() { return Buffer.Elements; }
		const	T* end() const { return Buffer.Elements + Count; }
		T* end() { return Buffer.Elements + Count; }

		bool IsFull() const { return Count == Buffer.Count; }

		void Init(Splice<T> splice)
		{
			Buffer = splice;
			Count = 0;
		}

		void InitAllocate(UInt capacity)
		{
			FE_CORE_ASSERT(!Buffer.Elements, "Not released SpliceArena - memory leak!");
			Buffer = Context::Allocators::Default->Allocate<T>(capacity);
			Count = 0;
		}

		void Release()
		{
			Context::Allocators::Default->Deallocate(Buffer);
			Buffer.Elements = nullptr;
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

		const	T* begin() const { return Buffer.Elements; }
		T* begin() { return Buffer.Elements; }
		const	T* end() const { return Buffer.Elements + Count; }
		T* end() { return Buffer.Elements + Count; }

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