#pragma once

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
		union RegElement;

		struct FreeListElement
		{
			FreeListElement* Next;
			U16 RegIndex;
		};

		struct Chunks
		{
			RegElement* ElementsChunk;
			U08* GenerationsChunk;
		};

		union RegElement
		{
			tObj mObj;
			FreeListElement mNext;
		};

		SpliceArena<Chunks> mChunks;

		FreeListElement* mFreeList = nullptr;
		TypedAlloc<Allocator>* AllocMain = nullptr;
		TypedAlloc<Allocator>* AllocAux = nullptr;

		InternalID GetNew()
		{
			InternalID result;
			result.mComps.Type = tObj::Type;

			if (mFreeList)
			{
				FreeListElement* result_ptr = mFreeList;
				mFreeList = mFreeList->FreeListNext;

				result.mComps.RegIndex = result_ptr->Index;

				unsigned long chunk_i;
				MSB64(&chunk_i, (U64)result_ptr->Index);
				auto chunk_mask = (U64)1 << chunk_i;
				auto in_chunk_i = result_ptr->Index - chunk_mask;
				auto generation = mChunks[chunk_i].GenerationsChunk + in_chunk_i;

				result.mComps.Generation = generation;

				return result;
			}

			if (Count == Capacity())
				Expand();

			Count++;

			unsigned long chunk_i;
			MSB64(&chunk_i, Count);
			auto chunk_mask = (U64)1 << chunk_i;
			auto in_chunk_i = Count - chunk_mask;
			auto result_ptr = Chunks[chunk_i] + in_chunk_i;

			return result_ptr;
		}
	};
}