#pragma once

#include "FoolsEngine/Foundation/Utils/Context.h"
#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Foundation/Memory/Arena.h"

#include "FoolsEngine/Renderer/1 - GAPI/GAPI.h"

#include <glad/gl.h>

namespace fe::GAPI::OpenGL
{
	union InternalID
	{
		GID mGID;
		struct
		{
			U08 Type;
			U08 Generation;
			U16 RegIndex;
		} mComps;
	};

	ObjType GetObjType(GID obj)
	{
		const InternalID& internalID = *(InternalID*)&obj;
		return (ObjType::ValueType)(internalID.mComps.Type);
	}

	template <typename tObj>
	struct Registry
	{
		// basically dynamic pool backed by xar for separatelly gapi objects and their generation trackers

		struct FreeListElement
		{
			FreeListElement* Next;
			U16 RegIndex;
		};

		union RegElement
		{
			tObj mObj;
			FreeListElement mFree;
		};

		struct Chunks
		{
			RegElement* ElementsChunk;
			U08* GenerationsChunk;
		};

		SpliceArena<Chunks> mChunks;

		FreeListElement* mFreeList = nullptr;
		TypedAlloc<Allocator>* mAllocMain = nullptr;
		TypedAlloc<Allocator>* mAllocAux = nullptr;

		void Create()
		{
			mAllocMain = Context::Allocators::Default;
			mAllocAux = Context::Allocators::Auxiliary;
		}

		InternalID GetNewID()
		{
			InternalID result;
			result.mComps.Type = tObj::Type;

			if (mFreeList)
			{
				FreeListElement* result_obj_ptr = mFreeList;
				mFreeList = mFreeList->FreeListNext;

				result.mComps.RegIndex = result_obj_ptr->RegIndex;

				unsigned long chunk_i;
				MSB64(&chunk_i, (U64)result_obj_ptr->RegIndex);
				auto chunk_mask = (U64)1 << chunk_i;
				auto in_chunk_i = result_obj_ptr->RegIndex - chunk_mask;
				auto generation = mChunks[chunk_i].GenerationsChunk[in_chunk_i];

				result.mComps.Generation = generation;

				return result;
			}
			
			if (mChunks.IsFull())
			{
				bool arena_any_capacity = mChunks.Count > 1;
				UInt arena_new_capacity = arena_any_capacity ? mChunks.Count + (mChunks.Count >> 1) : mChunks.Count + 1;

				Splice<Chunks> new_chunks = mAllocAux->Allocate<Chunks>(arena_new_capacity);

				UInt old_arena_size = mChunks.Buffer.Count * sizeof(Chunks);
				std::memcpy(new_chunks.Elements, mChunks.Buffer.Elements, old_arena_size);
				mAllocAux->Deallocate(mChunks.Buffer);

				mChunks.Buffer = new_chunks;
			}

			auto new_chunks = mChunks.PushBack();
			UInt new_chunk_capacity = UInt(1) << mChunks.Count;
			new_chunks->ElementsChunk = mAllocMain->Allocate<RegElement>(new_chunk_capacity).Elements;
			new_chunks->GenerationsChunk = mAllocMain->Allocate<U08>(new_chunk_capacity).Elements;

			new_chunks->GenerationsChunk[0] = 0;
			result.mComps.Generation = 0;
			result.mComps.RegIndex = reg_index_base;

			U16 reg_index_base = 2 ^ mChunks.Count;
			mFreeList = &(new_chunks->ElementsChunk[1].mFree);
			for (UInt i = 1; i < new_chunk_capacity; i++) // i=0 is result
			{
				new_chunks->ElementsChunk[i].mFree = { .Next = &(new_chunks->ElementsChunk[i+1].mFree), .RegIndex = reg_index_base + i };
				new_chunks->GenerationsChunk[i] = 0;
			}
			new_chunks->ElementsChunk[new_chunk_capacity - 1].mFree.Next = nullptr;

			mChunks.Count++;

			return result;
		}

		void FreeObj(InternalID id)
		{
			FE_CORE_ASSERT(id.mComps.Type == tObj::Type, "This GAPI object does not belong to this registry");

			unsigned long chunk_i;
			MSB64(&chunk_i, (U64)id.mComps.RegIndex);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = id.mComps.RegIndex - chunk_mask;
			auto& generation = mChunks[chunk_i].GenerationsChunk[in_chunk_i];

			FE_CORE_ASSERT(generation == id.mComps.Generation, "Double free in GAPI registry");
			
			generation++;

			auto& free_list_element = mChunks[chunk_i].ElementsChunk[in_chunk_i].mFree;
			free_list_element.Next = mFreeList;
			free_list_element.RegIndex = id.mComps.RegIndex;

			mFreeList = &free_list_element;
		}

		tObj* GetObj(InternalID id)
		{
			FE_CORE_ASSERT(id.mComps.Type == tObj::Type, "This GAPI object does not belong to this registry");
			
			unsigned long chunk_i;
			MSB64(&chunk_i, (U64)id.mComps.RegIndex);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = id.mComps.RegIndex - chunk_mask;
			auto& generation = mChunks[chunk_i].GenerationsChunk[in_chunk_i];
			
			FE_CORE_ASSERT(generation == id.mComps.Generation, "Allready freed from GAPI registry");

			auto& result = mChunks[chunk_i].ElementsChunk[in_chunk_i];

			return &result;
		}
	};
}