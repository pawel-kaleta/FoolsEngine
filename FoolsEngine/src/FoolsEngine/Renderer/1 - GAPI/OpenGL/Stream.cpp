#pragma once

#include "FE_pch.h"

#include "FoolsEngine/Renderer/1 - GAPI/Stream.h"

#include <glad/gl.h>


namespace fe::GAPI::Stream
{
	GID CreateDownStream(U32 size, U32 maxRegionCount)
	{

	}

	void AllocateDownStream(GID downStream)
	{

	}

	void DestroyDownStreamCmd(GID downStream)
	{

	}

	GID CreateRegion(GID stream, U32 size, U32 offsetAlignment = 16)
	{

	}

	Byte* GetRegionLocation(GID region)
	{

	}

	U32 GetRegionOffset(GID region)
	{

	}

	void CommitRegion(GID region)
	{

	}

	void RetireRegionCmd(GID region)
	{

	}
}