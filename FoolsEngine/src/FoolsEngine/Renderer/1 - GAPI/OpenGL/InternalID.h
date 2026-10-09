#pragma once

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Renderer/1 - GAPI/Context.h"

namespace fe::Render::GAPI::OpenGL
{
	union InternalID
	{
		InternalID() : mGID() {};
		InternalID(GID gid) : mGID(gid) {}
		operator GID() { return mGID; }
		GID mGID;
		struct
		{
			U08 Type;
			U08 Generation;
			U16 RegIndex;
		} mComps;
	};

	ObjType GetObjType(GID obj)
	{
		return (ObjType::ValueType)(InternalID(obj).mComps.Type);
	}
}