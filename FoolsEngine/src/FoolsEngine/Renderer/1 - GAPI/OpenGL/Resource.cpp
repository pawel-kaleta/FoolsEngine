#pragma once

#include "FE_pch.h"

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "Buffer.h"

#include <glad/gl.h>

namespace fe::GAPI::Resource
{
	GID CreateBuffer()
	{

	}

	Byte* AllocateBuffer(GID buffer, U32 size)
	{

	}

	void CommitBufferCmd(GID buffer)
	{

	}

	void DestroyBufferCmd(GID buffer)
	{

	}

	GID CreateShader(Descriptors::ShaderType type, CString source)
	{

	}

	void DestroyShader(GID shader)
	{

	}

	GID CreateTexture(const Descriptors::TextureSpec& spec)
	{

	}

	void AllocateTexture(GID texture)
	{

	}

	void CopyToTextureCmd(GID texture, GID region)
	{

	}

	void CopyToTextureCmd(GID texture, GID buffer, U32 offset)
	{

	}

	void DestroyTextureCmd(GID texture)
	{

	}

	GID CreateTextureViewCmd(GID texture, const Descriptors::TextureViewSpec& textureSpecView)
	{

	}

	GID DestroyTextureViewCmd(GID textureView)
	{

	}

	Data::std140_uvec2 GetTextureViewHandle(GID textureView)
	{
		
	}
}