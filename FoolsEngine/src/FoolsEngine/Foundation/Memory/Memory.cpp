#include "FE_pch.h"
#include "Array.h"
#include "Pile.h"
#include "Xar.h"

#include <memory_resource>
#include <vector>
#include <string>

namespace fe
{
	Byte	Pile::s_Buffer[Pile::s_BufferSize];
	U64		Pile::s_RollbackFlags;
	Byte*	Pile::s_Free = s_Buffer;
	U64		Pile::s_Count = 0;
#ifdef FE_INTERNAL_BUILD
	Byte*	Pile::s_MaxFree = s_Buffer;
#endif // FE_INTERNAL_BUILD
}