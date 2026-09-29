#pragma once

#include "FoolsEngine/Foundation/Utils/Context.h"
#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Foundation/Memory/Arena.h"
#include "FoolsEngine/Foundation/Memory/Splice.h"
#include "FoolsEngine/Foundation/Utils/BitOperations.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"

#include "GraphicsPipeline.h"
#include "Shader.h"
#include "Texture.h"
#include "DownStream.h"
#include "Buffer.h"

#include <cstring>

#include <glad/gl.h>

namespace fe::Render::GAPI::OpenGL
{
	union InternalID
	{
		InternalID() : mGID() {};
		InternalID(GID gid) : mGID(gid) {}
		operator GID() { return mGID; }
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
		return (ObjType::ValueType)(InternalID(obj).mComps.Type);
	}

	template <typename tObj>
	struct Registry
	{
		// basically dynamic pool backed by deque for separatelly gapi objects and their generation trackers

		struct Chunk
		{
			Array<tObj, 64>* Objs;
			Array<U08, 64>* Generations;
		};

		SpliceArena<Chunk> mChunks;
		Splice<U64> mOccupancyFlags; // true is free; "splicearena" not needed as its syncronised with mChunks

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
			result.mComps.Type = tObj::Type.Value;

			unsigned long index_IN_chunk;
			UInt index_OF_chunk = 0;
			bool found = false;
			for (; index_OF_chunk < mChunks.Count; index_OF_chunk++)
			{
				if (!mOccupancyFlags[index_OF_chunk])
				{
					continue;
				}
					
				MSB64(&index_IN_chunk, mOccupancyFlags[index_IN_chunk]);
						
				U64 flag_mask = (U64)1 << index_IN_chunk;
				mOccupancyFlags[index_OF_chunk] &= ~flag_mask;

				result.mComps.Generation = mChunks[index_OF_chunk].Generations->operator[](index_IN_chunk);
				result.mComps.RegIndex = index_OF_chunk * 64 + index_IN_chunk;

				return result;
			}
			
			// not found free spot, allocate 64 more

			// expand chunks arena and occupancyflags splice if needed
			if (mChunks.IsFull())
			{
				bool arena_any_capacity = mChunks.Count > 1;
				UInt arena_new_capacity = arena_any_capacity ? mChunks.Count + (mChunks.Count >> 1) : mChunks.Count + 1;

				Splice<Chunk> new_chunks = mAllocAux->Allocate<Chunk>(arena_new_capacity);

				UInt old_arena_size = mChunks.Buffer.Count * sizeof(Chunk);
				std::memcpy(new_chunks.Elements, mChunks.Buffer.Elements, old_arena_size);
				mAllocAux->Deallocate(mChunks.Buffer);

				mChunks.Buffer = new_chunks;


				mAllocAux->Deallocate(mOccupancyFlags);
				mOccupancyFlags = mAllocAux->Allocate<U64>(arena_new_capacity);

				// set occupancies to 0
				std::memset(mOccupancyFlags.Elements, 0, arena_new_capacity * 8);
			}

			// allocate new chunk arrays for objs and generations
			auto new_chunk = mChunks.PushBack();
			new_chunk->Objs = mAllocMain->Allocate<tObj, 64>();
			new_chunk->Generations = mAllocMain->Allocate<U08, 64>();
			
			// set generations to 0
			std::memset(new_chunk->Generations, 0, 64);

			//set occupancy flags of newly allocated chunk to 1 (exept the result obj)
			mOccupancyFlags.Elements[mChunks.Count - 1] = U64(-1) >> 1;

			result.mComps.RegIndex = (mChunks.Count - 1) * 64;
			result.mComps.Generation = 0;

			return result;
		}

		void FreeObj(InternalID id)
		{
			FE_CORE_ASSERT(id.mComps.Type == tObj::Type.Value, "This GAPI object does not belong to this registry");

			UInt index_OF_chunk = id.mComps.RegIndex / 64;
			UInt index_IN_chunk = id.mComps.RegIndex % 64;

			auto& generation = mChunks[index_OF_chunk].Generations->operator[](index_IN_chunk);

			FE_CORE_ASSERT(generation == id.mComps.Generation, "Double free in GAPI registry");
			
			generation++;
		}

		tObj* GetObj(InternalID id)
		{
			FE_CORE_ASSERT(id.mComps.Type == tObj::Type.Value, "This GAPI object does not belong to this registry");
			
			UInt index_OF_chunk = id.mComps.RegIndex / 64;
			UInt index_IN_chunk = id.mComps.RegIndex % 64;

			auto& generation = mChunks[index_OF_chunk].Generations->operator[](index_IN_chunk);

			if (generation == id.mComps.Generation)
				return & mChunks[index_OF_chunk].Objs->operator[](index_IN_chunk);
			
			FE_CORE_ASSERT(generation == id.mComps.Generation, "GAPI object allready removed from registry");

			return nullptr;
		}
	};

	extern Registry<GraphicsPipeline> GraphicsPipelineRegistry;
	extern Registry<Buffer> BufferRegistry;
	extern Registry<Texture> TextureRegistry;
	extern Registry<TextureView> TextureViewRegistry;
	extern Registry<Shader> ShaderRegistry;
	extern Registry<Region> RegionRegistry;
	extern Registry<Region> RegionRegistry;
	extern Registry<DownStream> DownStreamRegistry;

	void CreateRegistries();
}