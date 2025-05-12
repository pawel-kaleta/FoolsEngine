#include "FE_pch.h"

#include "Scratchpad.h"

namespace fe
{
	std::byte* Scratchpad::s_Free;
	uint64_t Scratchpad::s_RollbackFlags;
	uint32_t Scratchpad::s_Count;
	std::byte Scratchpad::s_Buffer[Scratchpad::s_BufferSize];
}