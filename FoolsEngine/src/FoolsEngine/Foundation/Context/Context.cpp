#include "FoolsEngine/Foundation/DataTypes.h"
#include "FoolsEngine/Foundation/Debug/Logger.h"
#include "FoolsEngine/Foundation/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Allocators/StaticArenaAlloc.h"
#include "FoolsEngine/Foundation/Allocators/MallocAlloc.h"
#include "FoolsEngine/Foundation/Allocators/MonotonicAlloc.h"
#include "FoolsEngine/Renderer/1 - GAPI/Context.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Platform/PlatformBase.h"

// TO BE THREAD LOCAL

namespace fe::Context
{
	namespace Alloc
	{
		TypedAlloc<MonotonicAlloc>		Permanent;
		TypedAlloc<MallocAlloc>			GeneralPurpose;
		TypedAlloc<StaticArenaAlloc<
			FE_PARAMS_MAIN_ARENA_SIZE>>	MainLoopArena;

		PMAlloc						*	Default		= (PMAlloc*) & GeneralPurpose;
		PMAlloc						*	Auxiliary	= (PMAlloc*) & GeneralPurpose;
		PMAlloc						*	Output		= (PMAlloc*) & GeneralPurpose;
	}

	namespace Log
	{
		Logger Core;
		Logger App;
	}

	namespace GPU
	{
		Render::GAPI::GID ScratchBuffer;
		constexpr U32 ScratchBufferSize = FE_PARAMS_GPU_SCRATCH_BUFFER_SIZE;
	}

	namespace Platform
	{
		PlatformBase Base;
	}
}

