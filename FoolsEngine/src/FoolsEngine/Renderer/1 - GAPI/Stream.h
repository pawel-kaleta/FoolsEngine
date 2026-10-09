#pragma once

#include "Context.h"

#include <numeric>

namespace fe::Render::GAPI
{
	GID		CreateDownStream();
	void	AllocateDownStream(GID downStream, U32 size);
	void	DestroyDownStreamCmd(GID downStream);

	GID		CreateRegion(GID stream, U32 size, U32 offsetAlignment = 16);
	void	CommitRegion(GID region);
	void	RetireRegionCmd(GID region);
	Byte*	GetRegionLocation(GID region);
	U32		GetRegionOffset(GID region);
	GID		GetStreamOfRegion(GID region);

	template <typename T>
	UInt GetOffsetAlignmentFor()
	{
		return std::lcm(16, std::lcm(alignof(T), sizeof(T)));
	}

	UInt GetOffsetAlignmentFor(UInt size)
	{
		return std::lcm(16, sizeof(size));
	}
}