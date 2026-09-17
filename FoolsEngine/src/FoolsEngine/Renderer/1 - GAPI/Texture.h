#pragma once

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "Descriptors.h"
#include "DownStream.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace fe::GAPI::Resource
{
	using namespace Descriptors::Texture;

	namespace Utils
	{
		GLenum FormatToGLFormat(Format format)
		{
			// TO DO: make this a static lookup table?

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
			case Format::DEPTH24STENCIL8:	FE_CORE_ASSERT(false, "Does this work?"); return GL_DEPTH24_STENCIL8;
			default:
				FE_CORE_ASSERT(false, "Uknown texture data format");
				return GL_NONE;
			}
		}

		GLenum FormatToGLInternalFormat(Format format)
		{
			// TO DO: make this a static lookup table?

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
			case Format::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
			default:
				FE_CORE_ASSERT(false, "Uknown texture data format");
				return GL_NONE;
			}
		}

		GLenum FormatToGLType(Format format)
		{
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
			case Format::DEPTH24STENCIL8:	FE_CORE_ASSERT(false, "Does this work?"); return GL_DEPTH24_STENCIL8; //this shouldnt be happening
			default:
				FE_CORE_ASSERT(false, "Uknown texture data format");
				return GL_NONE;
			}
		}
	}

	struct Texture
	{
		GLuint OpenGLID = 0;
		Specification Spec;

		void Make(const Specification& spec)
		{
			Spec = spec;

			switch (spec.mType.Value)
			{
			case Type::Texture1D:
				FE_CORE_ASSERT(false, "Texture type not implemented");
				break;
			case Type::Texture2D:
				glCreateTextures(GL_TEXTURE_2D, 1, &OpenGLID);			
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
			GLenum internal_format = Utils::FormatToGLInternalFormat(Spec.mFormat);
			glTextureStorage2D(OpenGLID, Spec.mMipCount, internal_format, Spec.mDimentions.x, Spec.mDimentions.y);
		}
	};

	void CopyToTextureCmd(DownStream::Region* region, Texture* texture)
	{
		auto& spec = texture->Spec;

		auto& dim = texture->Spec.mDimentions;
		GLenum format = Utils::FormatToGLFormat(spec.mFormat);
		GLenum type = Utils::FormatToGLType(spec.mFormat);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, region->Stream->OpenGLBuffer);
		glTextureSubImage2D(texture->OpenGLID, 0, 0, 0, dim.x, dim.y, format, type, region->Data);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		glGenerateTextureMipmap(texture->OpenGLID);
	}

	// bindless stuff here
}