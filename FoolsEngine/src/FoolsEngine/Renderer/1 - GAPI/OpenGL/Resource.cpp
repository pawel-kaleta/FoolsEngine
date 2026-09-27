#pragma once

#include "FE_pch.h"

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Registry.h"

#include <glad/gl.h>

namespace fe::GAPI::Resource
{
	GID CreateBuffer()
	{
		OpenGL::InternalID id = OpenGL::BufferRegistry.GetNewID();
		OpenGL::BufferRegistry.GetObj(id)->Init();
		return id;
	}

	Byte* AllocateBuffer(GID buffer, U32 size)
	{
		return OpenGL::BufferRegistry.GetObj(buffer)->Allocate(size);
	}

	void AllocateCommit(GID buffer, U32 size)
	{
		OpenGL::BufferRegistry.GetObj(buffer)->AllocateCommit(size);
	}

	void CommitBuffer(GID buffer)
	{
		OpenGL::BufferRegistry.GetObj(buffer)->Commit();
	}

	void DestroyBufferCmd(GID buffer)
	{
		OpenGL::Buffer* obj = OpenGL::BufferRegistry.GetObj(buffer);
		obj->ReleaseCmd();
		OpenGL::BufferRegistry.FreeObj(buffer);
	}

	GID CreateShader(Descriptors::ShaderType type, CString source)
	{
		OpenGL::InternalID id = OpenGL::ShaderRegistry.GetNewID();
		OpenGL::Shader* shader = OpenGL::ShaderRegistry.GetObj(id);
		shader->Create(source, type);
		return id;
	}

	void DestroyShader(GID shader)
	{
		OpenGL::Shader* obj = OpenGL::ShaderRegistry.GetObj(shader);
		obj->Destroy();
		OpenGL::ShaderRegistry.FreeObj(shader);
	}

	GID CreateTexture(const Descriptors::TextureSpec& spec)
	{
		OpenGL::InternalID id = OpenGL::TextureRegistry.GetNewID();
		OpenGL::Texture* texture = OpenGL::TextureRegistry.GetObj(id);
		texture->Create(spec);
		return id;
	}

	void AllocateTexture(GID texture)
	{
		OpenGL::Texture* obj = OpenGL::TextureRegistry.GetObj(texture);
		obj->Allocate();
	}

	void CopyToTextureCmd(GID texture, GID region)
	{
		OpenGL::Texture*	texture_obj	= OpenGL::TextureRegistry.GetObj(texture);
		OpenGL::Region*		region_obj	= OpenGL::RegionRegistry.GetObj(region);

		auto& spec = texture_obj->Spec;

		auto& dim = texture_obj->Spec.mDimentions;
		GLenum format = OpenGL::Utils::FormatToGLFormat(spec.mFormat);
		GLenum type = OpenGL::Utils::FormatToGLType(spec.mFormat);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, region_obj->mStream->OpenGLBuffer);
		glTextureSubImage2D(texture_obj->OpenGLID, 0, 0, 0, dim.x, dim.y, format, type, region_obj->mData);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		glGenerateTextureMipmap(texture_obj->OpenGLID);
	}

	void CopyToTextureCmd(GID texture, GID buffer, U32 offset)
	{
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj(texture);
		OpenGL::Buffer* buffer_obj = OpenGL::BufferRegistry.GetObj(buffer);

		auto& spec = texture_obj->Spec;

		auto& dim = texture_obj->Spec.mDimentions;
		GLenum format = OpenGL::Utils::FormatToGLFormat(spec.mFormat);
		GLenum type = OpenGL::Utils::FormatToGLType(spec.mFormat);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer_obj->mGLID);
		glTextureSubImage2D(texture_obj->OpenGLID, 0, 0, 0, dim.x, dim.y, format, type, (void*)offset);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		glGenerateTextureMipmap(texture_obj->OpenGLID);
	}

	void DestroyTextureCmd(GID texture)
	{
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj(texture);
		texture_obj->DestroyCmd();
		OpenGL::TextureRegistry.FreeObj(texture);
	}

	GID CreateTextureView(GID texture, const Descriptors::TextureViewSpec& textureSpecView)
	{
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj(texture);

		OpenGL::InternalID textureview_id = OpenGL::TextureViewRegistry.GetNewID();
		OpenGL::TextureView* textureview_obj = OpenGL::TextureViewRegistry.GetObj(textureview_id);

		textureview_obj->Init();
		textureview_obj->Create(textureSpecView, texture_obj);

		return textureview_id;
	}

	void CommitTextureViewCmd(GID textureView)
	{
		OpenGL::TextureView* textureview_obj = OpenGL::TextureViewRegistry.GetObj(textureView);
		textureview_obj->CommitCmd();
	}

	void DestroyTextureViewCmd(GID textureView)
	{
		OpenGL::TextureView* textureview_obj = OpenGL::TextureViewRegistry.GetObj(textureView);
		textureview_obj->DestroyCmd();
		OpenGL::TextureViewRegistry.FreeObj(textureView);
	}

	Data::std140_uvec2 GetTextureViewHandle(GID textureView)
	{
		OpenGL::TextureView* textureview_obj = OpenGL::TextureViewRegistry.GetObj(textureView);
		return (Data::std140_uvec2)(textureview_obj->mTextureSamplerHandleGL);
	}
}