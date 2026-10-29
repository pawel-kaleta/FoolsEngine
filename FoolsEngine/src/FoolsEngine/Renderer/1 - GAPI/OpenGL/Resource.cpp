#include "FoolsEngine/Foundation/DataTypes.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Registry.h"
#include "Framebuffer.h"


#include <glad/gl.h>

namespace fe::Render::GAPI
{
	GID CreateBuffer()
	{
		OpenGL::InternalID id = OpenGL::BufferRegistry.GetNewID();
		OpenGL::BufferRegistry.GetObj(id)->Init();
		return id;
	}

	Byte* AllocateBuffer(GID buffer, U32 size)
	{
		FE_PROFILER_FUNC();
		return OpenGL::BufferRegistry.GetObj(buffer)->Allocate(size);
	}

	void AllocateCommitBuffer(GID buffer, U32 size)
	{
		FE_PROFILER_FUNC();
		OpenGL::BufferRegistry.GetObj(buffer)->AllocateCommit(size);
	}

	void CommitBuffer(GID buffer)
	{
		FE_PROFILER_FUNC();
		OpenGL::BufferRegistry.GetObj(buffer)->Commit();
	}

	void DestroyBufferCmd(GID buffer)
	{
		FE_PROFILER_FUNC();
		OpenGL::Buffer* obj = OpenGL::BufferRegistry.GetObj(buffer);
		obj->ReleaseCmd();
		OpenGL::BufferRegistry.FreeObj(buffer);
	}

	void GAPI::CopyRegionCmd(GID sourceBuffer, U32 sourceOffset, U32 size, GID targetBuffer, U32 targetOffset)
	{
		FE_PROFILER_FUNC();
		OpenGL::Buffer* source_obj = OpenGL::BufferRegistry.GetObj(sourceBuffer);
		OpenGL::Buffer* target_obj = OpenGL::BufferRegistry.GetObj(targetBuffer);

		FE_CORE_ASSERT(source_obj->mSize <= sourceOffset + size, "AAAA");
		FE_CORE_ASSERT(target_obj->mSize <= targetOffset + size, "AAAA");

		glCopyNamedBufferSubData(source_obj->mGLID, target_obj->mGLID, sourceOffset, targetOffset, size);
	}

	GID CreateShader(Descriptors::ShaderType type, CString source)
	{
		FE_PROFILER_FUNC();
		OpenGL::InternalID id = OpenGL::ShaderRegistry.GetNewID();
		OpenGL::Shader* shader = OpenGL::ShaderRegistry.GetObj(id);
		shader->Create(source, type);
		return id;
	}

	void DestroyShader(GID shader)
	{
		FE_PROFILER_FUNC();
		OpenGL::Shader* obj = OpenGL::ShaderRegistry.GetObj(shader);
		obj->Destroy();
		OpenGL::ShaderRegistry.FreeObj(shader);
	}

	GID CreateTexture(const Descriptors::TextureSpec& spec)
	{
		FE_PROFILER_FUNC();
		OpenGL::InternalID id = OpenGL::TextureRegistry.GetNewID();
		OpenGL::Texture* texture = OpenGL::TextureRegistry.GetObj(id);
		texture->Create(spec);
		return id;
	}

	GID CreateTexture(const Descriptors::TextureSpec& spec, const Descriptors::TextureViewSpec& viewSpec)
	{
		FE_PROFILER_FUNC();
		OpenGL::InternalID id = OpenGL::TextureRegistry.GetNewID();
		OpenGL::Texture* texture = OpenGL::TextureRegistry.GetObj(id);
		texture->Create(spec, viewSpec);
		return id;
	}

	void AllocateTexture(GID texture)
	{
		FE_PROFILER_FUNC();
		OpenGL::Texture* obj = OpenGL::TextureRegistry.GetObj(texture);
		obj->Allocate();
	}

	void CopyToTextureCmd(GID texture, GID region)
	{
		FE_PROFILER_FUNC();
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj(texture);
		OpenGL::Region* region_obj = OpenGL::RegionRegistry.GetObj(region);

		auto& spec = texture_obj->mSpec;

		auto& dim = texture_obj->mSpec.mDimentions;
		GLenum format = OpenGL::Utils::FormatToGLFormat(spec.mFormat);
		GLenum type = OpenGL::Utils::FormatToGLType(spec.mFormat);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, region_obj->mStream->mOpenGLBuffer);
		glTextureSubImage2D(texture_obj->mOpenGLID, 0, 0, 0, dim.x, dim.y, format, type, (void*)(UInt)region_obj->GetOffset());
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		glGenerateTextureMipmap(texture_obj->mOpenGLID);
	}

	void CopyToTextureCmd(GID texture, GID buffer, U32 offset)
	{
		FE_PROFILER_FUNC();
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj(texture);
		OpenGL::Buffer* buffer_obj = OpenGL::BufferRegistry.GetObj(buffer);

		auto& spec = texture_obj->mSpec;

		auto& dim = texture_obj->mSpec.mDimentions;
		GLenum format = OpenGL::Utils::FormatToGLFormat(spec.mFormat);
		GLenum type = OpenGL::Utils::FormatToGLType(spec.mFormat);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer_obj->mGLID);
		glTextureSubImage2D(texture_obj->mOpenGLID, 0, 0, 0, dim.x, dim.y, format, type, (void*)offset);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		glGenerateTextureMipmap(texture_obj->mOpenGLID);
	}

	void Clear(GID texture, Splice<U32> values)
	{
		FE_PROFILER_FUNC();
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj(texture);

		texture_obj->ClearCmd(values);
	}

	void Clear(GID texture, Splice<F32> values)
	{
		FE_PROFILER_FUNC();
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj(texture);

		texture_obj->ClearCmd(values);
	}

	void DestroyTextureCmd(GID texture)
	{
		FE_PROFILER_FUNC();
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj(texture);
		texture_obj->DestroyCmd();
		OpenGL::TextureRegistry.FreeObj(texture);
	}

	UInt GetTextureNativeID(GID texture)
	{
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj(texture);
		return texture_obj->mOpenGLID;
	}

	GID CreateTextureView(GID texture, const Descriptors::TextureViewSpec& textureViewSpec)
	{
		FE_PROFILER_FUNC();
		OpenGL::Texture* texture_obj = OpenGL::TextureRegistry.GetObj(texture);

		OpenGL::InternalID textureview_id = OpenGL::TextureViewRegistry.GetNewID();
		OpenGL::TextureView* textureview_obj = OpenGL::TextureViewRegistry.GetObj(textureview_id);

		textureview_obj->Init();
		textureview_obj->Create(textureViewSpec, texture_obj);

		return textureview_id;
	}

	void CommitTextureViewCmd(GID textureView)
	{
		FE_PROFILER_FUNC();
		OpenGL::TextureView* textureview_obj = OpenGL::TextureViewRegistry.GetObj(textureView);
		textureview_obj->CommitCmd();
	}

	void DestroyTextureViewCmd(GID textureView)
	{
		FE_PROFILER_FUNC();
		OpenGL::TextureView* textureview_obj = OpenGL::TextureViewRegistry.GetObj(textureView);
		textureview_obj->DestroyCmd();
		OpenGL::TextureViewRegistry.FreeObj(textureView);
	}

	std140_uvec2 GetTextureViewHandle(GID textureView)
	{
		OpenGL::TextureView* textureview_obj = OpenGL::TextureViewRegistry.GetObj(textureView);

		return *(std140_uvec2*)&(textureview_obj->mTextureSamplerHandleGL);
	}

	GID CreateFramebuffer(const Descriptors::FramebufferSpec& framebuffer, UInt width, UInt height)
	{

	}

	void ResizeFramebuffer(GID framebuffer, UInt width, UInt height)
	{

	}

	void DestroyFramebuffer(GID framebuffer)
	{

	}
}