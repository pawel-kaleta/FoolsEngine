#include "FE_pch.h"
#include "Utils.h"

namespace fe::Resource
{
	namespace Utils
	{
		GLenum DataPrimitiveToGLBaseType(Description::Data::Primitive primitive)
		{
			const static GLenum s_lookup_table[] = { GL_BOOL, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_DOUBLE };
			return s_lookup_table[primitive.ToInt() - 1];
		};

		GLenum FormatToGLFormat(Description::Texture::Format format)
		{
			// TO DO: make this a static lookup table?

			switch (format.Value)
			{
			case Description::Texture::Format::None:
				FE_CORE_ASSERT(false, "Not specified data format of attachment");
				return GL_NONE;
			case Description::Texture::Format::R_8:				return GL_RED;
			case Description::Texture::Format::RG_8:			return GL_RG;
			case Description::Texture::Format::RGB_8:			return GL_RGB;
			case Description::Texture::Format::RGBA_8:			return GL_RGBA;
			case Description::Texture::Format::R_UINT_32:		return GL_RED_INTEGER;
			case Description::Texture::Format::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
			default:
				FE_CORE_ASSERT(false, "Uknown data format of attachment");
				return GL_NONE;
			}
		}

		GLenum FormatToGLInternalFormat(Description::Texture::Format format)
		{
			// TO DO: make this a static lookup table?

			switch (format.Value)
			{
			case Description::Texture::Format::None:
				FE_CORE_ASSERT(false, "Not specified data format of attachment");
				return GL_NONE;
			case Description::Texture::Format::R_8:				return GL_R8;
			case Description::Texture::Format::RG_8:			return GL_RG8;
			case Description::Texture::Format::RGB_8:			return GL_RGB8;
			case Description::Texture::Format::RGBA_8:			return GL_RGBA8;
			case Description::Texture::Format::R_UINT_32:		return GL_R32UI;
			case Description::Texture::Format::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
			default:
				FE_CORE_ASSERT(false, "Uknown data format of attachment");
				return GL_NONE;
			}
		}

		GLenum FormatToGLType(Description::Texture::Format format)
		{
			switch (format.Value)
			{
			case Description::Texture::Format::None:
				FE_CORE_ASSERT(false, "Not specified data format of attachment");
				return GL_NONE;
			case Description::Texture::Format::R_8:				return GL_UNSIGNED_BYTE;
			case Description::Texture::Format::RG_8:			return GL_UNSIGNED_BYTE;
			case Description::Texture::Format::RGB_8:			return GL_UNSIGNED_BYTE;
			case Description::Texture::Format::RGBA_8:			return GL_UNSIGNED_BYTE;
			case Description::Texture::Format::R_UINT_32:		return GL_UNSIGNED_INT;
			case Description::Texture::Format::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
			default:
				FE_CORE_ASSERT(false, "Uknown data format of attachment");
				return GL_NONE;
			}
		}

		GLenum ShaderTypeToGLEnum(Description::ShaderInterface::ShaderType type)
		{
			switch (type)
			{
			case Description::ShaderInterface::ShaderType::None:		return GL_NONE;
			case Description::ShaderInterface::ShaderType::Vertex:		return GL_VERTEX_SHADER;
			case Description::ShaderInterface::ShaderType::Fragment:	return GL_FRAGMENT_SHADER;
			default:
				FE_LOG_CORE_ERROR("Unrecognized shader type");
				return GL_NONE;
			}
		}


	}
}