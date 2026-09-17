#include "FE_pch.h"
#include "Splice.h"
#include "Pile.h"
#include "Xar.h"

#include <memory_resource>
#include <vector>
#include <string>

namespace fe
{
	Byte	PileBase::s_Buffer[PileBase::s_BufferSize];
	U64		PileBase::s_RollbackFlags;
	Byte*	PileBase::s_Free = s_Buffer;
	U64		PileBase::s_Count = 0;
#ifdef FE_INTERNAL_BUILD
	Byte*	PileBase::s_MaxFree = s_Buffer;
#endif // FE_INTERNAL_BUILD
}