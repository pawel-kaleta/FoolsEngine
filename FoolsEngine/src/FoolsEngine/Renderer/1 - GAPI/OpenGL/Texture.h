#pragma once

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"

#include "Buffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/gl.h>

namespace fe::GAPI::OpenGL
{
	using namespace Resource::Descriptors;

	namespace Utils
	{
		GLenum FormatToGLFormat(TextureFormat format)
		{
			// TO DO: make this a static lookup table?

			switch (format.Value)
			{
			case TextureFormat::None:
				FE_CORE_ASSERT(false, "Not specified texture data format");
				return GL_NONE;
			case TextureFormat::R_8:				return GL_RED;
			case TextureFormat::RG_8:				return GL_RG;
			case TextureFormat::RGB_8:				return GL_RGB;
			case TextureFormat::RGBA_8:				return GL_RGBA;
			case TextureFormat::R_UINT_32:			return GL_RED_INTEGER;
			case TextureFormat::DEPTH24STENCIL8:	FE_CORE_ASSERT(false, "Does this work?"); return GL_DEPTH24_STENCIL8;
			default:
				FE_CORE_ASSERT(false, "Uknown texture data format");
				return GL_NONE;
			}
		}

		GLenum FormatToGLInternalFormat(TextureFormat format)
		{
			// TO DO: make this a static lookup table?

			switch (format.Value)
			{
			case TextureFormat::None:
				FE_CORE_ASSERT(false, "Not specified texture data format");
				return GL_NONE;
			case TextureFormat::R_8:				return GL_R8;
			case TextureFormat::RG_8:				return GL_RG8;
			case TextureFormat::RGB_8:				return GL_RGB8;
			case TextureFormat::RGBA_8:				return GL_RGBA8;
			case TextureFormat::R_UINT_32:			return GL_R32UI;
			case TextureFormat::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
			default:
				FE_CORE_ASSERT(false, "Uknown texture data format");
				return GL_NONE;
			}
		}

		GLenum FormatToGLType(TextureFormat format)
		{
			switch (format.Value)
			{
			case TextureFormat::None:
				FE_CORE_ASSERT(false, "Not specified texture data format");
				return GL_NONE;
			case TextureFormat::R_8:				return GL_UNSIGNED_BYTE;
			case TextureFormat::RG_8:				return GL_UNSIGNED_BYTE;
			case TextureFormat::RGB_8:				return GL_UNSIGNED_BYTE;
			case TextureFormat::RGBA_8:				return GL_UNSIGNED_BYTE;
			case TextureFormat::R_UINT_32:			return GL_UNSIGNED_INT;
			case TextureFormat::DEPTH24STENCIL8:	FE_CORE_ASSERT(false, "Does this work?"); return GL_DEPTH24_STENCIL8; //this shouldnt be happening
			default:
				FE_CORE_ASSERT(false, "Uknown texture data format");
				return GL_NONE;
			}
		}
	}

	struct Texture
	{
		constexpr static ObjType Type = ObjType::Texture;

		GLuint OpenGLID = 0;
		TextureSpec Spec;

		void Init()
		{
			OpenGLID = 0;
			Spec = TextureSpec();
		}

		void Create(const TextureSpec& spec)
		{
			Spec = spec;

			switch (spec.mType.Value)
			{
			case TextureType::Texture1D:
				FE_CORE_ASSERT(false, "Texture type not implemented");
				break;
			case TextureType::Texture2D:
				glCreateTextures(GL_TEXTURE_2D, 1, &OpenGLID);			
				break;
			case TextureType::Texture3D:
				FE_CORE_ASSERT(false, "Texture type not implemented");
				break;
			case TextureType::Texture2DArray:
				FE_CORE_ASSERT(false, "Texture type not implemented");
				break;
			default:
				FE_CORE_ASSERT(false, "Texture type not recognised");
			}
		}

		void Allocate()
		{
			GLenum internal_format = Utils::FormatToGLInternalFormat(Spec.mFormat);
			glTextureStorage2D(OpenGLID, Spec.mMipCount, internal_format, Spec.mDimentions.x, Spec.mDimentions.y);
		}

		void DestroyCmd()
		{
			glDeleteTextures(1, &OpenGLID);
		}
	};

	struct TextureView
	{
		constexpr static ObjType Type = ObjType::TextureView;

		TextureViewSpec mSampler;
		Texture* mTexture = nullptr;
		GLuint mSamplerID = 0;
		GLuint64 mTextureSamplerHandleGL = -1;

		void Init()
		{
			mSampler = TextureViewSpec();
			mTexture = nullptr;
			mSamplerID = 0;
			mTextureSamplerHandleGL = -1;
		}

		void Create(const TextureViewSpec& sampler, Texture* texture)
		{
			mSampler = sampler;
			mTexture = texture;

			glGenSamplers(1, &mSamplerID);

			if (sampler.mASFiltering != ASFiltering::None)
			{
				GLfloat anisotropy = 2 ^ sampler.mASFiltering.Value;
				glSamplerParameterf(mSamplerID, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
			}

			if (sampler.mWrapping != Wrapping::Repeat) // default opengl
			{
				switch (sampler.mWrapping.Value)
				{
				case Wrapping::MirrorRepeat:
					glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
					glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
					break;
				case Wrapping::Clamp:
					glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					break;
				case Wrapping::Border:
					glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
					glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
					glSamplerParameterfv(mSamplerID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(sampler.mBorderColor));
					break;
				default:
					FE_LOG_CORE_ERROR("Unrecognized texture wrapping mode, defaulted to repeat");
				}
			}

			if (sampler.mFiltering != Filtering::Nearest || sampler.mMipmapping != Mipmapping::Liniear)
			{
				switch (sampler.mFiltering.Value)
				{
				case Filtering::Nearest:
					glSamplerParameteri(mSamplerID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					switch (sampler.mMipmapping.Value)
					{
					case Mipmapping::None:		glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST); break;
					case Mipmapping::Nearest:	glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); break;
					case Mipmapping::Liniear:	glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); break;
					default: FE_LOG_CORE_ERROR("Unrecognized texture Mipmapping mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR");
					}
					break;
				case Filtering::Bilinear:
					glSamplerParameteri(mSamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					switch (sampler.mMipmapping.Value)
					{
					case Mipmapping::None:		glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
					case Mipmapping::Nearest:	glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); break;
					case Mipmapping::Liniear:	glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); break;
					default: FE_LOG_CORE_ERROR("Unrecognized texture Mipmapping mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR");
					}
					break;
				default:
					FE_LOG_CORE_ERROR("Unrecognized texture Filtering mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR");
				}
			}
		}

		void CommitCmd()
		{
			mTextureSamplerHandleGL = glGetTextureSamplerHandleARB(mTexture->OpenGLID, mSamplerID);

			glMakeTextureHandleResidentARB(mTextureSamplerHandleGL);
		}

		void DestroyCmd()
		{
			glMakeTextureHandleNonResidentARB(mTextureSamplerHandleGL);
			glDeleteSamplers(1, &mSamplerID);

			mSamplerID = -1;
			mTextureSamplerHandleGL = -1;
		}
	};
}