#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"

namespace fe::Render::GAPI
{
	FE_DECLARE_ENUM(Platform, None, OpenGL, Vulkan);

	struct GID
	{
		bool operator==(const GID& other) const { return this->mID == other.mID; }
	private:
		U32 mID = -1;
	};

	FE_DECLARE_ENUM(ObjType, None, Buffer, DownStream, UpStream, Region, Texture, TextureView, Shader, GraphicsPipeline, ComputePipeline);

	ObjType GetObjType(GID obj);


	bool Create();

	void SetViewportCmd(U32 x, U32 y, U32 width, U32 height);
}