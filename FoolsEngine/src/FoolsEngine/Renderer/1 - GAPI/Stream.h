#pragma once

#include "GAPI.h"

namespace fe::GAPI::Stream
{
	GID		CreateDownStream(U32 size, U32 maxRegionCount);
	void	AllocateDownStream(GID downStream);
	void	DestroyDownStreamCmd(GID downStream);

	GID		CreateRegion(GID stream, U32 size, U32 offsetAlignment = 16);
	Byte*	GetRegionLocation(GID region);
	U32		GetRegionOffset(GID region);
	void	CommitRegion();
	void	RetireRegionCmd();

	template <typename T>
	UInt GetOffsetAlligmentFor()
	{
		return std::lcm(16, std::lcm(alignof(T), sizeof(T)));
	}
}