#pragma once

#include "FoolsEngine/Foundation/Utils/Context.h"
#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Foundation/Memory/Arena.h"
#include "FoolsEngine/Foundation/Memory/Splice.h"
#include "FoolsEngine/Foundation/Utils/BitOperations.h"
#include "FoolsEngine/Foundation/Utils/TypeSetup.h"


#include "FoolsEngine/Renderer/1 - GAPI/Context.h"

#include "GraphicsPipeline.h"
#include "Shader.h"
#include "Texture.h"
#include "DownStream.h"
#include "Buffer.h"
#include "InternalID.h"

#include <glad/gl.h>

namespace fe::Render::GAPI::OpenGL
{
	template <typename tObj>
	struct Registry
	{
		// basically dynamic pool for separatelly gapi objects and their generation trackers
		// with unified occupancy tracking uasing bit flags instead of free list

		struct Chunk
		{
			Array<tObj, 64>* Objs;
			Array<U08, 64>* Generations;
			U64 OccupancyFlags; // true is free;
		};

		DynamicArena<Chunk> mChunks;

		PMAlloc* mAllocMain = nullptr;

		void Create()
		{
			mAllocMain = Context::Allocators::Default;
			mChunks.Init(Context::Allocators::Auxiliary);
		}

		InternalID GetNewID()
		{
			InternalID result;
			result.mComps.Type = tObj::Type.Value;

			for (UInt i = 0; i < mChunks.Count; i++)
			{
				Chunk& chunk = mChunks[i];

				if (!chunk.OccupancyFlags)
					continue;

				unsigned long index_IN_chunk;
				MSB64(&index_IN_chunk, chunk.OccupancyFlags);

				U64 flag_mask = (U64)1 << index_IN_chunk;
				chunk.OccupancyFlags &= ~flag_mask;

				result.mComps.Generation = chunk.Generations->At(index_IN_chunk);
				result.mComps.RegIndex = i * 64 + index_IN_chunk;

				return result;
			}
			
			// not found free spot, allocate 64 more

			auto new_chunk = mChunks.EmplaceBack();
			new_chunk->Objs = mAllocMain->Allocate<tObj, 64>();
			new_chunk->OccupancyFlags = U64(-1) >> 1;
			new_chunk->Generations = mAllocMain->Allocate<U08, 64>();
			std::memset(new_chunk->Generations, 0, 64);
			
			result.mComps.RegIndex = (mChunks.Count - 1) * 64;
			result.mComps.Generation = 0;

			return result;
		}

		void FreeObj(InternalID id)
		{
			FE_CORE_ASSERT(id.mComps.Type == tObj::Type.Value, "This GAPI object does not belong to this registry");

			UInt index_OF_chunk = id.mComps.RegIndex / 64;
			UInt index_IN_chunk = id.mComps.RegIndex % 64;

			auto& generation = mChunks[index_OF_chunk].Generations->At(index_IN_chunk);

			FE_CORE_ASSERT(generation == id.mComps.Generation, "Double free in GAPI registry");
			
			generation++;
		}

		tObj* GetObj(InternalID id)
		{
			FE_CORE_ASSERT(id.mComps.Type == tObj::Type.Value, "This GAPI object does not belong to this registry");
			
			UInt index_OF_chunk = id.mComps.RegIndex / 64;
			UInt index_IN_chunk = id.mComps.RegIndex % 64;

			auto& generation = mChunks[index_OF_chunk].Generations->At(index_IN_chunk);

			if (generation == id.mComps.Generation)
				return mChunks[index_OF_chunk].Objs->At(index_IN_chunk).P();
			
			FE_CORE_ASSERT(generation == id.mComps.Generation, "GAPI object allready removed from registry");

			return nullptr;
		}
	};

	extern Registry<GraphicsPipeline>	GraphicsPipelineRegistry;
	extern Registry<Buffer>				BufferRegistry;
	extern Registry<Texture>			TextureRegistry;
	extern Registry<TextureView>		TextureViewRegistry;
	extern Registry<Shader>				ShaderRegistry;
	extern Registry<Region>				RegionRegistry;
	extern Registry<DownStream>			DownStreamRegistry;

	extern void CreateRegistries();
}