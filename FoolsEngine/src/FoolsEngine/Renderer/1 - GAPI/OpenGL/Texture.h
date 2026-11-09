#pragma once

#include "FoolsEngine/Foundation/DataTypes.h"
#include "FoolsEngine/Foundation/Utils/TypeSetup.h"

#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/gl.h>

namespace fe::Render::GAPI::OpenGL
{

	namespace Utils
	{
		inline GLenum FormatToGLFormat(Descriptors::Texture::Format format)
		{
			// TO DO: make this a static lookup table?
			using namespace Descriptors::Texture;

			switch (format.Value)
			{
			case Format::None:
				FE_CORE_ASSERT(false, "Not specified texture data format");
				return GL_NONE;
			case Format::R_8:				return GL_RED;
			case Format::RG_8:				return GL_RG;
			case Format::RGB_8:				return GL_RGB;
			case Format::RGBA_8:			return GL_RGBA;
			case Format::R_UINT_32:			return GL_RED_INTEGER;
			case Format::DEPTH24_STENCIL8:	FE_CORE_ASSERT(false, "What are you trying to do?"); return GL_DEPTH_COMPONENT;
			case Format::DEPTH32F_STENCIL8:	FE_CORE_ASSERT(false, "What are you trying to do?"); return GL_DEPTH_COMPONENT;
			default:
				FE_CORE_ASSERT(false, "Uknown texture data format");
				return GL_NONE;
			}
		}

		inline GLenum FormatToGLInternalFormat(Descriptors::Texture::Format format)
		{
			// TO DO: make this a static lookup table?
			using namespace Descriptors::Texture;

			switch (format.Value)
			{
			case Format::None:
				FE_CORE_ASSERT(false, "Not specified texture data format");
				return GL_NONE;
			case Format::R_8:				return GL_R8;
			case Format::RG_8:				return GL_RG8;
			case Format::RGB_8:				return GL_RGB8;
			case Format::RGBA_8:			return GL_RGBA8;
			case Format::R_UINT_32:			return GL_R32UI;
			case Format::DEPTH24_STENCIL8:	return GL_DEPTH24_STENCIL8;
			case Format::DEPTH32F_STENCIL8:	return GL_DEPTH32F_STENCIL8;

			default:
				FE_CORE_ASSERT(false, "Uknown texture data format");
				return GL_NONE;
			}
		}

		inline GLenum FormatToGLType(Descriptors::Texture::Format format)
		{
			// TO DO: make this a static lookup table?
			using namespace Descriptors::Texture;

			switch (format.Value)
			{
			case Format::None:
				FE_CORE_ASSERT(false, "Not specified texture data format");
				return GL_NONE;
			case Format::R_8:				return GL_UNSIGNED_BYTE;
			case Format::RG_8:				return GL_UNSIGNED_BYTE;
			case Format::RGB_8:				return GL_UNSIGNED_BYTE;
			case Format::RGBA_8:			return GL_UNSIGNED_BYTE;
			case Format::R_UINT_32:			return GL_UNSIGNED_INT;
			case Format::DEPTH24_STENCIL8:	FE_CORE_ASSERT(false, "what are you trying to do?"); return GL_UNSIGNED_INT; //this shouldnt be happening (why?)
			case Format::DEPTH32F_STENCIL8:	FE_CORE_ASSERT(false, "what are you trying to do?"); return GL_UNSIGNED_INT; //this shouldnt be happening (why?)
			default:
				FE_CORE_ASSERT(false, "Uknown texture data format");
				return GL_NONE;
			}
		}
	}

	struct Texture
	{
		constexpr static ObjType Type = ObjType::Texture;


		FE_TYPE_SETUP(Texture)

		GLuint mOpenGLID = 0;
		Descriptors::Texture::Spec mSpec;
		Descriptors::Texture::Sampling::ViewSpec mViewSpec;
		GLenum mGLType;
		GLenum mGLFormat;
		GLenum mGLInternalFormat;

		void Init()
		{
			mOpenGLID = 0;
			mSpec.Init();
		}

		void Create(const Descriptors::Texture::Spec& spec)
		{
			using namespace Descriptors::Texture;
			mSpec = spec;
			mGLType = Utils::FormatToGLType(spec.mFormat);
			mGLFormat = Utils::FormatToGLFormat(spec.mFormat);
			mGLInternalFormat = Utils::FormatToGLInternalFormat(spec.mFormat);

			switch (spec.mType.Value)
			{
			case Type::Texture1D:
				FE_CORE_ASSERT(false, "Texture type not implemented");
				break;
			case Type::Texture2D:
				glCreateTextures(GL_TEXTURE_2D, 1, &mOpenGLID);			
				break;
			case Type::Texture3D:
				FE_CORE_ASSERT(false, "Texture type not implemented");
				break;
			case Type::Texture2DArray:
				FE_CORE_ASSERT(false, "Texture type not implemented");
				break;
			default:
				FE_CORE_ASSERT(false, "Texture type not recognised");
			}
		}

		void Create(const Descriptors::Texture::Spec& spec, const Descriptors::Texture::Sampling::ViewSpec& viewSpec)
		{
			using namespace Descriptors::Texture;

			mSpec = spec;
			mGLType = Utils::FormatToGLType(spec.mFormat);
			mGLFormat = Utils::FormatToGLFormat(spec.mFormat);
			mGLInternalFormat = Utils::FormatToGLInternalFormat(spec.mFormat);
			mViewSpec = viewSpec;

			switch (spec.mType.Value)
			{
			case Type::Texture1D:
				FE_CORE_ASSERT(false, "Texture type not implemented");
				break;
			case Type::Texture2D:
				glCreateTextures(GL_TEXTURE_2D, 1, &mOpenGLID);
				break;
			case Type::Texture3D:
				FE_CORE_ASSERT(false, "Texture type not implemented");
				break;
			case Type::Texture2DArray:
				FE_CORE_ASSERT(false, "Texture type not implemented");
				break;
			default:
				FE_CORE_ASSERT(false, "Texture type not recognised");
			}
		}

		void Allocate()
		{
			using namespace Descriptors::Texture::Sampling;
			if (mViewSpec.mASFiltering != ASFiltering::None)
			{
				GLfloat anisotropy = 2 ^ mViewSpec.mASFiltering.Value;
				glTextureParameterf(mOpenGLID, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
			}

			if (mViewSpec.mWrapping != Wrapping::Repeat) // default opengl
			{
				switch (mViewSpec.mWrapping.Value)
				{
				case Wrapping::MirrorRepeat:
					glTextureParameteri(mOpenGLID, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
					glTextureParameteri(mOpenGLID, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				break;
				case Wrapping::Clamp:
					glTextureParameteri(mOpenGLID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTextureParameteri(mOpenGLID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
				case Wrapping::Border:
					glTextureParameteri(mOpenGLID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
					glTextureParameteri(mOpenGLID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
					glTextureParameterfv(mOpenGLID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(mViewSpec.mBorderColor));
				break;
				default:
					Context::Log::Core.Error("Unrecognized texture wrapping mode, defaulted to repeat"_FE);
				}
			}

			if (mViewSpec.mFiltering != Filtering::Nearest || mViewSpec.mMipmapping != Mipmapping::Liniear)
			{
				switch (mViewSpec.mFiltering.Value)
				{
				case Filtering::Nearest:
					glTextureParameteri(mOpenGLID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					switch (mViewSpec.mMipmapping.Value)
					{
					case Mipmapping::None:		glTextureParameteri(mOpenGLID, GL_TEXTURE_MIN_FILTER, GL_NEAREST); mSpec.mMipCount = 1; break;
					case Mipmapping::Nearest:	glTextureParameteri(mOpenGLID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); mSpec.CalculateMipCount(); break;
					case Mipmapping::Liniear:	glTextureParameteri(mOpenGLID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); mSpec.CalculateMipCount(); break;
					default: Context::Log::Core.Error("Unrecognized texture Mipmapping mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR"_FE); mSpec.CalculateMipCount();
					}
				break;
				case Filtering::Bilinear:
					glTextureParameteri(mOpenGLID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					switch (mViewSpec.mMipmapping.Value)
					{
					case Mipmapping::None:		glTextureParameteri(mOpenGLID, GL_TEXTURE_MIN_FILTER, GL_LINEAR); mSpec.CalculateMipCount(); break;
					case Mipmapping::Nearest:	glTextureParameteri(mOpenGLID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); mSpec.CalculateMipCount(); break;
					case Mipmapping::Liniear:	glTextureParameteri(mOpenGLID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); mSpec.CalculateMipCount(); break;
					default: Context::Log::Core.Error("Unrecognized texture Mipmapping mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR"_FE); mSpec.CalculateMipCount();
					}
				break;
				default:
					Context::Log::Core.Info("Unrecognized texture Filtering mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR"_FE);
					mSpec.CalculateMipCount();
				}
			}

			glTextureStorage2D(mOpenGLID, mSpec.mMipCount, mGLInternalFormat, mSpec.mDimentions.x, mSpec.mDimentions.y);
		}

		void ClearCmd(Splice<U32> values)
		{
			glClearTexImage(mOpenGLID, 0, mGLFormat, GL_UNSIGNED_INT, values.Elements);
		}

		void ClearCmd(Splice<F32> values)
		{		
			glClearTexImage(mOpenGLID, 0, mGLFormat, GL_UNSIGNED_INT, values.Elements);
		}

		void DestroyCmd()
		{
			glDeleteTextures(1, &mOpenGLID);
		}
	};

	struct TextureView
	{
		constexpr static ObjType Type = ObjType::TextureView;

		Descriptors::Texture::Sampling::ViewSpec mSampler;
		Texture* mTexture = nullptr;
		GLuint mSamplerID = 0;
		GLuint64 mTextureSamplerHandleGL = -1;

		FE_TYPE_SETUP(TextureView)

		void Init()
		{
			mSampler.Init();
			mTexture = nullptr;
			mSamplerID = 0;
			mTextureSamplerHandleGL = -1;
		}

		void Create(const Descriptors::Texture::Sampling::ViewSpec& sampler, Texture* texture)
		{
			using namespace Descriptors::Texture::Sampling;
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
					Context::Log::Core.Warn("Unrecognized texture wrapping mode, defaulted to repeat"_FE);
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
					default: Context::Log::Core.Error("Unrecognized texture Mipmapping mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR"_FE);
					}
					break;
				case Filtering::Bilinear:
					glSamplerParameteri(mSamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					switch (sampler.mMipmapping.Value)
					{
					case Mipmapping::None:		glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
					case Mipmapping::Nearest:	glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); break;
					case Mipmapping::Liniear:	glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); break;
					default: Context::Log::Core.Error("Unrecognized texture Mipmapping mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR"_FE);
					}
					break;
				default:
					Context::Log::Core.Warn("Unrecognized texture Filtering mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR"_FE);
				}
			}
		}

		void CommitCmd()
		{
			mTextureSamplerHandleGL = glGetTextureSamplerHandleARB(mTexture->mOpenGLID, mSamplerID);

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