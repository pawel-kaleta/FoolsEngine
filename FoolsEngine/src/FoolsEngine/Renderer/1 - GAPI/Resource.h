#pragma once

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Foundation/Memory/String.h"

#include "Context.h"
#include "Data.h"

#include <glm/glm.hpp>

namespace fe::Render::GAPI
{
	namespace Descriptors
	{
		FE_DECLARE_ENUM(ShaderType, None, Vertex, Fragment, Compute);
		
		FE_DECLARE_ENUM(TextureType, None, Texture1D, Texture2D, Texture3D, Texture2DArray);

		FE_DECLARE_ENUM(TextureFormat, None, R_8, RG_8, RGB_8, RGBA_8, R_UINT_32, DEPTH24STENCIL8);

		struct TextureSpec
		{
			TextureType mType = TextureType::None;
			TextureFormat mFormat = TextureFormat::None;
			glm::vec<3, U32> mDimentions = { 0, 0, 0 };
			U32 mMipCount = 1;
			U32 mLayerCount = 1;

			void CalculateMipCount()
			{
				mMipCount = glm::log2((float)glm::max(mDimentions.x, mDimentions.y));
			}

			// UsageFlags mUsageFlags = UsageFlags::None;  ?? Sampled, Storage, ColorAttachment, DepthStencilAttachment
		};

		FE_DECLARE_ENUM(Wrapping, None, Repeat, MirrorRepeat, Clamp, Border);

		FE_DECLARE_ENUM(Filtering, None, Nearest, Bilinear);

		FE_DECLARE_ENUM(Mipmapping, None, Nearest, Liniear);

		FE_DECLARE_ENUM(ASFiltering, None, x2, x4, x8, x16);

		struct TextureViewSpec
		{
			Wrapping	mWrapping = Wrapping::None;
			Filtering	mFiltering = Filtering::None;
			Mipmapping	mMipmapping = Mipmapping::None;
			ASFiltering	mASFiltering = ASFiltering::None;
			glm::vec4	mBorderColor = { 0.0, 0.0, 0.0, 0.0 };
		};
	}


	GID		CreateBuffer();
	Byte*	AllocateBuffer(GID buffer, U32 size);
	void	AllocateCommitBuffer(GID buffer, U32 size);
	void	CommitBuffer(GID buffer);
	void	DestroyBufferCmd(GID buffer);


	GID		CreateShader(Descriptors::ShaderType type, CString source);
	void	DestroyShader(GID shader);


	GID		CreateTexture(const Descriptors::TextureSpec& spec);
	void	AllocateTexture(GID texture);
	void	CopyToTextureCmd(GID texture, GID region);
	void	CopyToTextureCmd(GID texture, GID buffer, U32 offset);
	void	Clear(GID texture, Splice<U32> values);
	void	Clear(GID texture, Splice<F32> values);
	void	DestroyTextureCmd(GID texture);

	GID		CreateTextureView(GID texture, const Descriptors::TextureViewSpec& textureViewSpec);
	void	CommitTextureViewCmd(GID textureView);
	std140_uvec2 GetTextureViewHandle(GID textureView);
	void	DestroyTextureViewCmd(GID textureView);
}