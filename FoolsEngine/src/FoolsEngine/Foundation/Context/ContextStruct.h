#pragma once

#include "FoolsEngine/Foundation/Debug/Logger.h"

#include "FoolsEngine/Foundation/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Allocators/StaticArenaAlloc.h"
#include "FoolsEngine/Foundation/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Allocators/MonotonicAlloc.h"

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"

#include "FoolsEngine/Platform/PlatformBase.h"

namespace fe
{
	static constexpr UInt arena_size = FE_PARAMS_MAIN_ARENA_SIZE;

	struct ContextStruct
	{
		struct
		{
			TypedAlloc<MonotonicAlloc>			* Permanent;
			TypedAlloc<MallocAlloc>				* GeneralPurpose;
			TypedAlloc<
				StaticArenaAlloc<arena_size>>	* MainLoopArena;

			PMAlloc* Default;
			PMAlloc* Auxiliary;
			PMAlloc* Output;
		} Alloc;

		struct
		{
			Logger FE;
			Logger AP;
			Logger ED;
		} Log;

		struct
		{

		} Debug;

		struct
		{

		} Rand;

		struct
		{
			Render::GAPI::GID ScratchBuffer;
			static constexpr U32 ScratchBufferSize = FE_PARAMS_GPU_SCRATCH_BUFFER_SIZE;
		} GPU;
	};
}