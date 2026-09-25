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

		stream->Init(id);
		return id;
	}

	void AllocateDownStream(GID downStream, U32 size)
	{
		OpenGL::DownStream* stream = OpenGL::DownStreamRegistry.GetObj(downStream);
		stream->Create(size);
	}

	void DestroyDownStreamCmd(GID downStream)
	{
		OpenGL::DownStream* stream = OpenGL::DownStreamRegistry.GetObj(downStream);
		stream->ReleaseCmd();
	}

	GID CreateRegion(GID stream, U32 size, U32 offsetAlignment = 16)
	{
		OpenGL::InternalID id = stream;
		if (id.mComps.Type == ObjType::DownStream)
		{
			OpenGL::DownStream* stream_ptr = OpenGL::DownStreamRegistry.GetObj(id);

			OpenGL::InternalID region_id = stream_ptr->CreateRegion(size, offsetAlignment);

			return region_id;
		}
		if (id.mComps.Type == ObjType::UpStream)
		{
			//OpenGL::UpStream* stream_ptr = OpenGL::DownStreamRegistry.GetObj(id);
			FE_CORE_ASSERT(false, "Not implemented!!");
			return GID();
		}

		FE_CORE_ASSERT(false, "Aaaaa!!");
		return GID();
	}

	Byte* GetRegionLocation(GID region)
	{
		OpenGL::Region* region_obj = OpenGL::RegionRegistry.GetObj(region);
		return region_obj->mData;
	}

	U32 GetRegionOffset(GID region)
	{
		OpenGL::Region* region_obj = OpenGL::RegionRegistry.GetObj(region);
		return region_obj->GetOffset();
	}

	void CommitRegion(GID region)
	{
		OpenGL::Region* region_obj = OpenGL::RegionRegistry.GetObj(region);
		OpenGL::Stream* stream_obj = region_obj->mStream;
		if (stream_obj->mID.mComps.Type == ObjType::DownStream)
		{
			((OpenGL::DownStream * )stream_obj)->CommitRegion(region_obj);
			return;
		}
		if (stream_obj->mID.mComps.Type == ObjType::UpStream)
		{
			FE_CORE_ASSERT(false, "Not implemented!!");
			return;
		}

		FE_CORE_ASSERT(false, "Aaaaa!!");
	}

	void RetireRegionCmd(GID region)
	{
		OpenGL::Region* region_obj = OpenGL::RegionRegistry.GetObj(region);
		OpenGL::Stream* stream_obj = region_obj->mStream;
		if (stream_obj->mID.mComps.Type == ObjType::DownStream)
		{
			((OpenGL::DownStream*)stream_obj)->RetireRegionCmd(region_obj);
			return;
		}
		if (stream_obj->mID.mComps.Type == ObjType::UpStream)
		{
			FE_CORE_ASSERT(false, "Not implemented!!");
			return;
		}

		FE_CORE_ASSERT(false, "Aaaaa!!");
	}
}