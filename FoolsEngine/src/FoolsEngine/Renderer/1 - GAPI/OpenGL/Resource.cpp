#pragma once

#include "FE_pch.h"

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Registry.h"


#include <glad/gl.h>

namespace fe::GAPI::OpenGL
{
	Registry<Buffer> BufferRegistry;
	Registry<Texture> TextureRegistry;
	Registry<TextureView> TextureViewRegistry;
	Registry<Shader> ShaderRegistry;
	Registry<Region> RegionRegistry;

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

	void CommitBuffer(GID buffer)
	{
		OpenGL::InternalID id = (OpenGL::InternalID)buffer;
		OpenGL::BufferRegistry.GetObj(id)->Commit();
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
		OpenGL::InternalID id = OpenGL::ShaderRegistry.GetNewID();
		OpenGL::Shader* shader = OpenGL::ShaderRegistry.GetObj(id);
		shader->Create(source, type);
		return id.mGID;
	}

	void DestroyShader(GID shader)
	{
		OpenGL::InternalID id = (OpenGL::InternalID)shader;
		OpenGL::Shader* obj = OpenGL::ShaderRegistry.GetObj(id);
		obj->Destroy();
		OpenGL::ShaderRegistry.FreeObj(id);
	}

	GID CreateTexture(const Descriptors::TextureSpec& spec)
	{
		OpenGL::InternalID id = OpenGL::TextureRegistry.GetNewID();
		OpenGL::Texture* texture = OpenGL::TextureRegistry.GetObj(id);
		texture->Create(spec);
		return id.mGID;
	}

	void AllocateTexture(GID texture)
	{
		OpenGL::InternalID id = (OpenGL::InternalID)texture;
		OpenGL::Texture* obj = OpenGL::TextureRegistry.GetObj(id);
		obj->Allocate();
	}

	void CopyToTextureCmd(GID texture, GID region)
	{
		OpenGL::Texture*	texture_obj	= OpenGL::TextureRegistry.GetObj((OpenGL::InternalID)texture);
		OpenGL::Region*		region_obj	= OpenGL::RegionRegistry.GetObj((OpenGL::InternalID)region);


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
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj((OpenGL::InternalID)texture);
		OpenGL::Buffer* buffer_obj = OpenGL::BufferRegistry.GetObj((OpenGL::InternalID)buffer);

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
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj((OpenGL::InternalID)texture);
		texture_obj->DestroyCmd();
		OpenGL::TextureRegistry.FreeObj((OpenGL::InternalID)texture);
	}

	GID CreateTextureView(GID texture, const Descriptors::TextureViewSpec& textureSpecView)
	{
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj((OpenGL::InternalID)texture);

		OpenGL::InternalID textureview_id = OpenGL::TextureViewRegistry.GetNewID();
		OpenGL::TextureView* textureview_obj = OpenGL::TextureViewRegistry.GetObj(textureview_id);

		textureview_obj->Init();
		textureview_obj->Create(textureSpecView, texture_obj);

		return textureview_id.mGID;
	}

	void CommitTextureViewCmd(GID textureView)
	{
		OpenGL::TextureView* textureview_obj = OpenGL::TextureViewRegistry.GetObj((OpenGL::InternalID)textureView);
		textureview_obj->CommitCmd();
	}

	void DestroyTextureViewCmd(GID textureView)
	{
		OpenGL::TextureView* textureview_obj = OpenGL::TextureViewRegistry.GetObj((OpenGL::InternalID)textureView);
		textureview_obj->DestroyCmd();
		OpenGL::TextureViewRegistry.FreeObj((OpenGL::InternalID)textureView);
	}

	Data::std140_uvec2 GetTextureViewHandle(GID textureView)
	{
		OpenGL::TextureView* textureview_obj = OpenGL::TextureViewRegistry.GetObj((OpenGL::InternalID)textureView);
		return (Data::std140_uvec2)(textureview_obj->mTextureSamplerHandleGL);
	}
}