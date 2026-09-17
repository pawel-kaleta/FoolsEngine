#pragma once

#include "Splice.h"
#include "FoolsEngine/Foundation/Utils/Context.h"

#include <cstring>

namespace fe
{
	template <typename T>
	struct Pool
	{
		union PoolElement
		{
			T Element;
			PoolElement* NextFree;
		};
		static_assert(sizeof(T) == sizeof(PoolElement));

		Splice<PoolElement> Buffer;
		PoolElement* FreeList = nullptr;

		bool IsFull() const { return !FreeList; }

		void Init(Splice<T> splice)
		{
			Buffer = *(Splice<PoolElement>*)&splice;
			FreeList = Buffer.Elements;

			for (auto& element : Buffer)
			{
				element.NextFree = (&element) + 1;
			}

			Buffer[splice.Count - 1].NextFree = nullptr;
		}

		void InitAllocate(UInt capacity)
		{
			FE_CORE_ASSERT(!Buffer.Elements, "Not released Pool - memory leak!");
			Buffer = Context::Allocators::Default->Allocate<PoolElement>(capacity);
			FreeList = Buffer.Elements;

			for (auto& element : Buffer)
			{
				element.NextFree = (&element) + 1;
			}

			Buffer[capacity - 1].NextFree = nullptr;
		}

		void Release()
		{
			Context::Allocators::Default->Deallocate(Buffer);
			Buffer.Elements = nullptr;
			FreeList = nullptr;
		}

		T* Emplace()
		{
			FE_CORE_ASSERT(FreeList, "Pool is full.");

			auto free = FreeList;
			FreeList = FreeList->NextFree;
			return (T*)free;
		}

		void Remove(T* element)
		{
			((PoolElement*)element)->NextFree = FreeList;
			FreeList = (PoolElement*)element;
			return;
		}
	};
}