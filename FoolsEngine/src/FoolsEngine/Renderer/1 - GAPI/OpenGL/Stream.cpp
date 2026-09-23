#pragma once

#include "FE_pch.h"

#include "FoolsEngine/Renderer/1 - GAPI/Stream.h"

#include "DownStream.h"
#include "Registry.h"

#include <glad/gl.h>


namespace fe::GAPI::OpenGL
{
	Registry<DownStream> DownStreamRegistry;
	Registry<Region> RegionRegistry;

	// TO DO: Create() on all registries
}

namespace fe::GAPI::Stream
{
	GID CreateDownStream()
	{
		OpenGL::InternalID id = OpenGL::DownStreamRegistry.GetNewID();
		OpenGL::DownStream* stream = OpenGL::DownStreamRegistry.GetObj(id);

		stream->Init();
		return id.mGID;
	}

	void AllocateDownStream(GID downStream, U32 size)
	{
		OpenGL::DownStream* stream = OpenGL::DownStreamRegistry.GetObj((OpenGL::InternalID)downStream);
		stream->Create(size);
	}

	void DestroyDownStreamCmd(GID downStream)
	{
		OpenGL::DownStream* stream = OpenGL::DownStreamRegistry.GetObj((OpenGL::InternalID)downStream);
		stream->ReleaseCmd();
	}

	GID CreateRegion(GID stream, U32 size, U32 offsetAlignment = 16)
	{
		OpenGL::InternalID id = (OpenGL::InternalID)stream;
		if (id.mComps.Type == ObjType::DownStream)
		{
			OpenGL::DownStream* stream_ptr = OpenGL::DownStreamRegistry.GetObj(id);

			OpenGL::InternalID region_id = stream_ptr->CreateRegion(size, offsetAlignment);

			return region_id.mGID;
		}
		if (id.mComps.Type == ObjType::UpStream)
		{
			//OpenGL::UpStream* stream_ptr = OpenGL::DownStreamRegistry.GetObj(id);

			return;
		}

		FE_CORE_ASSERT(false, "Aaaaa!!");
		return GID();
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