#include "FoolsEngine/Foundation/DataTypes.h"
#include "FoolsEngine/Foundation/Debug/Logger.h"
#include "FoolsEngine/Foundation/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Allocators/ArenaAlloc.h"
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
		TypedAlloc<ArenaAlloc>			MainLoopArena;
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
		U32 ScratchBufferSize;
	}

	namespace Platform
	{
		PlatformBase Base;
	}
}

