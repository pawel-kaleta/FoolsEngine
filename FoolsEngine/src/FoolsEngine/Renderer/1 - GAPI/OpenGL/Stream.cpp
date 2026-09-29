#pragma once

#include "FE_pch.h"

#include "FoolsEngine/Renderer/1 - GAPI/Stream.h"

#include "Registry.h"
#include "DownStream.h"

#include <glad/gl.h>


namespace fe::Render::GAPI
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

			OpenGL::InternalID region_id = OpenGL::RegionRegistry.GetNewID();
			OpenGL::Region* region_ptr = OpenGL::RegionRegistry.GetObj(region_id);

			bool success = stream_ptr->CreateRegion(region_ptr, size, offsetAlignment);

			if (!success)
			{
				OpenGL::RegionRegistry.FreeObj(region_id);
				return GID();
			}

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
			OpenGL::RegionRegistry.FreeObj(region);

			return;
		}
		if (stream_obj->mID.mComps.Type == ObjType::UpStream)
		{
			FE_CORE_ASSERT(false, "Not implemented!!");
			return;
		}

		FE_CORE_ASSERT(false, "Aaaaa!!");
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

	GID GetStreamOfRegion(GID region)
	{
		OpenGL::Region* region_obj = OpenGL::RegionRegistry.GetObj(region);
		return region_obj->mStream->mID;
	}
}