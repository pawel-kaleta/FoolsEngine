#pragma once

#include "FoolsEngine/Foundation/DataTypes.h"
#include "FoolsEngine/Foundation/Containers/Arena.h"
#include "FoolsEngine/Foundation/Containers/String.h"

#include "Context.h"
#include "Data.h"
#include "Descriptors.h"

#include <glm/glm.hpp>


namespace fe::Render::GAPI
{
	GID		CreateBuffer();
	Byte*	AllocateBuffer(GID buffer, U32 size);
	void	AllocateCommitBuffer(GID buffer, U32 size);
	void	CommitBuffer(GID buffer);
	void	DestroyBufferCmd(GID buffer);


	void	CopyRegionCmd(GID sourceBuffer, U32 sourceOffset, U32 sourceSize, GID targetBuffer, U32 targetOffset);


	GID		CreateShader(Descriptors::ShaderType type, CString source);
	void	DestroyShader(GID shader);


	GID		CreateTexture(const Descriptors::Texture::Spec& spec);
	GID		CreateTexture(const Descriptors::Texture::Spec& spec, const Descriptors::Texture::Sampling::ViewSpec& viewSpec);
	void	AllocateTexture(GID texture);
	void	CopyToTextureCmd(GID texture, GID region);
	void	CopyToTextureCmd(GID texture, GID buffer, U32 offset);
	void	Clear(GID texture, Splice<U32> values);
	void	Clear(GID texture, Splice<F32> values);
	void	DestroyTextureCmd(GID texture);
	UInt	GetTextureNativeID(GID texture);

	GID		CreateTextureView(GID texture, const Descriptors::Texture::Sampling::ViewSpec& textureViewSpec);
	void	CommitTextureViewCmd(GID textureView);
	std140_uvec2 GetTextureViewHandle(GID textureView);
	void	DestroyTextureViewCmd(GID textureView);


	GID		CreateFramebufferCmd(const Descriptors::Raster::FramebufferSpec& spec, UInt width, UInt height);
	void	ResizeFramebufferCmd(GID framebuffer, UInt width, UInt height);
	void	DestroyFramebufferCmd(GID framebuffer);
	UInt	GetAttachmentNativeID(GID framebuffer, UInt index);
	UInt	GetDepthAttachmentNativeID(GID framebuffer);

}