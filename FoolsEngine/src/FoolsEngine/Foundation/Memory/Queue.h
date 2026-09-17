#pragma once

#include "Splice.h"
#include "FoolsEngine/Foundation/Utils/Context.h"

#include <cstring>

namespace fe
{
	template <typename T>
	struct Queue
	{
		Splice<T> Buffer;
		UInt Front = 0;
		UInt Count = 0;

		bool IsFull() const { return Count == Buffer.Count; }
		bool IsEmpty() const { return Count; }

		void Init(Splice<T> splice)
		{
			Buffer = splice;
			Count = 0;
			Front = 0;
		}

		void InitAllocate(UInt capacity)
		{
			FE_CORE_ASSERT(!Buffer.Elements, "Not released Queue - memory leak!");
			Buffer = Context::Allocators::Default->Allocate<T>(capacity);
			Count = 0;
			Front = 0;
		}

		void Release()
		{
			Context::Allocators::Default->Deallocate(Buffer);
			Buffer.Elements = nullptr;
		}

		T& operator[](UInt i)
		{
			FE_CORE_ASSERT(i < Count, "Out of Queue bound!");

			return Buffer[(Front + i) % Buffer.Count];
		}

		const T& operator[](UInt i) const
		{
			FE_CORE_ASSERT(i < Count, "Out of Queue bound!");
			return Buffer[(Front + i) % Buffer.Count];
		}

		T* First()
		{
			if (!Count)
				return nullptr;

			return &Buffer[Front];
		}

		T* Last()
		{
			if (!Count)
				return nullptr;

			return &Buffer[(Front + Count - 1) % Buffer.Count];
		}

		T* AppendBack()
		{
			FE_CORE_ASSERT(Count < Buffer.Count, "Queue overflow!");
			T* result = &Buffer[(Front + Count) % Buffer.Count];
			Count++;
			return result;
		}

		T PopBack()
		{
			FE_CORE_ASSERT(Count, "Queue is empty!");
			Count--;
			return Buffer[(Front + Count) % Buffer.Count];
		}

		T* AppendFront()
		{
			FE_CORE_ASSERT(Count < Buffer.Count, "Queue overflow!");
			Front = (Front - 1 + Buffer.Count) % Buffer.Count;
			T* result = &Buffer[Front];
			Count++;
			return result;
		}

		T PopFront()
		{
			FE_CORE_ASSERT(Count, "Queue is empty!");
			Count--;
			return Buffer[Front];
		}
	};
}