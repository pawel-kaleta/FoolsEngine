#pragma once

#include "Splice.h"
#include "Arena.h"
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

		PoolElement* FreeList = nullptr;
		Splice<PoolElement> Buffer;

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

		void InitAllocate(UInt capacity, Allocator* alloc = Context::Allocators::Default)
		{
			FE_CORE_ASSERT(!Buffer.Elements, "Not released Pool - memory leak!");
			Buffer = ((PMAlloc*)alloc)->Allocate<PoolElement>(capacity);
			FreeList = Buffer.Elements;

			for (auto& element : Buffer)
			{
				element.NextFree = (&element) + 1;
			}

			Buffer[capacity - 1].NextFree = nullptr;
		}

		void Release(Allocator* alloc = Context::Allocators::Default)
		{
			((PMAlloc*)alloc)->Deallocate(Buffer);
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

	template <typename T, UInt chunkCapacity>
	struct DynamicPool
	{
		union PoolElement
		{
			T Element;
			PoolElement* NextFree;
		};

		PMAlloc* MainAlloc;
		PoolElement* FreeList;
		DynamicArena< Array<PoolElement, chunkCapacity>* > Chunks;

		bool IsFull() const { return !FreeList; }

		void Init(Allocator* mainAlloc = Context::Allocators::Default, Allocator* auxAlloc = Context::Allocators::Auxiliary)
		{
			MainAlloc = (PMAlloc*) mainAlloc;
			Chunks.Init(auxAlloc);
			FreeList = nullptr;
		}

		void Clear()
		{
			if (Chunks.Count == 0)
				return;

			FreeList = &( Chunks[0]->At(0) );

			for (UInt i = 0; i < Chunks.Count - 1; i++)
			{
				for (UInt j = 0; j < chunkCapacity - 1; j++)
				{
					PoolElement* elem = &( Chunks[i]->At(j) );
					elem->NextFree = elem + 1;
				}
				PoolElement* last_in_chunk = &( Chunks[i]->At(chunkCapacity - 1) );
				last_in_chunk->NextFree = &( Chunks[i + 1]->At(0) );
			}

			UInt last_chunk_index = Chunks.Count - 1;
			for (UInt j = 0; j < chunkCapacity - 1; j++)
			{
				PoolElement* elem = &( Chunks[last_chunk_index]->At(j) );
				elem->NextFree = elem + 1;
			}
			PoolElement* last_in_chunk = &( Chunks[last_chunk_index]->At(chunkCapacity - 1) );
			last_in_chunk->NextFree = nullptr;
		}

		void Release()
		{
			for (auto& chunk : Chunks)
			{
				MainAlloc->Deallocate(chunk);
			}

			Chunks.Release();
			FreeList = nullptr;
		}

		T* Emplace()
		{
			if (!FreeList)
			{
				auto new_chunk = MainAlloc->Allocate<PoolElement, chunkCapacity>();
				Chunks.Append(new_chunk);
				for (UInt i = 1; i < chunkCapacity - 1; i++)
				{
					PoolElement* elem = & new_chunk[i];
					elem->NextFree = ++elem;
				}
				FreeList = & new_chunk[1];
				new_chunk[chunkCapacity - 1].NextFree = nullptr;

				return &new_chunk[0];
			}

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