#pragma once

#include "FE_pch.h"

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "Buffer.h"
#include "Texture.h"
#include "Registry.h"

#include <glad/gl.h>

namespace fe::GAPI::OpenGL
{
	Registry<Buffer> BufferRegistry;
	Registry<Texture> TextureRegistry;
	Registry<TextureView> TextureViewRegistry;

	// TO DO: Create() on all regiestries
}

namespace fe::GAPI::Resource
{
	GID CreateBuffer()
	{
		OpenGL::InternalID id = OpenGL::BufferRegistry.GetNewID();
		OpenGL::BufferRegistry.GetObj(id)->Init();
		return id.mGID;
	}

	Byte* AllocateBuffer(GID buffer, U32 size)
	{
		OpenGL::InternalID id = (OpenGL::InternalID)buffer;
		
		return OpenGL::BufferRegistry.GetObj(id)->Allocate(size);
	}

	void AllocateCommit(GID buffer, U32 size)
	{
		OpenGL::InternalID id = (OpenGL::InternalID)buffer;
		OpenGL::BufferRegistry.GetObj(id)->AllocateCommit(size);
	}

	void CommitBufferCmd(GID buffer)
	{
		OpenGL::InternalID id = (OpenGL::InternalID)buffer;
		OpenGL::BufferRegistry.GetObj(id)->CommitCmd();
	}

	void DestroyBufferCmd(GID buffer)
	{
		OpenGL::InternalID id = (OpenGL::InternalID)buffer;
		OpenGL::Buffer* obj = OpenGL::BufferRegistry.GetObj(id);
		obj->ReleaseCmd();
		OpenGL::BufferRegistry.FreeObj(id);
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