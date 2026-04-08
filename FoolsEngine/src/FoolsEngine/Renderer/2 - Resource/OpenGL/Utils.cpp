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

		//GL_UNSIGNED_BYTE
		//GL_BYTE
		//GL_UNSIGNED_SHORT
		//GL_SHORT
		//GL_UNSIGNED_INT
		//GL_INT
		//GL_HALF_FLOAT
		//GL_FLOAT
		//GL_UNSIGNED_BYTE_3_3_2
		//GL_UNSIGNED_BYTE_2_3_3_REV
		//GL_UNSIGNED_SHORT_5_6_5
		//GL_UNSIGNED_SHORT_5_6_5_REV
		//GL_UNSIGNED_SHORT_4_4_4_4
		//GL_UNSIGNED_SHORT_4_4_4_4_REV
		//GL_UNSIGNED_SHORT_5_5_5_1
		//GL_UNSIGNED_SHORT_1_5_5_5_REV
		//GL_UNSIGNED_INT_8_8_8_8
		//GL_UNSIGNED_INT_8_8_8_8_REV
		//GL_UNSIGNED_INT_10_10_10_2
		//GL_UNSIGNED_INT_2_10_10_10_REV

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
			case Description::Texture::Format::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8; //this shouldnt be happening
			default:
				FE_CORE_ASSERT(false, "Uknown data format of attachment");
				return GL_NONE;
			}
		}

		GLenum ShaderTypeToGLEnum(Description::ShaderInterface::ShaderType type)
		{
			switch (type)
			{
			case Description::ShaderInterface::ShaderType::None:
				FE_CORE_ASSERT(false, "Not specified Shader Type");
				return GL_NONE;
			case Description::ShaderInterface::ShaderType::Vertex:		return GL_VERTEX_SHADER;
			case Description::ShaderInterface::ShaderType::Fragment:	return GL_FRAGMENT_SHADER;
			default:
				FE_LOG_CORE_ERROR("Unrecognized shader type");
				return GL_NONE;
			}
		}
		
		GLenum BufferUsageToGLEnum(Description::Buffer::Usage usage)
		{
			switch (usage)
			{
			case Description::Buffer::Usage::None:
				FE_CORE_ASSERT(false, "Not specified buffer usage");
				return GL_NONE;
			case Description::Buffer::Usage::Vertex:		return GL_ARRAY_BUFFER;
			case Description::Buffer::Usage::Index:			return GL_ELEMENT_ARRAY_BUFFER;
			case Description::Buffer::Usage::IndexVertex:	return GL_ARRAY_BUFFER;
			case Description::Buffer::Usage::Batch:			return GL_ARRAY_BUFFER;
			case Description::Buffer::Usage::Uniform:		return GL_UNIFORM;
			case Description::Buffer::Usage::ShaderStorage:	return GL_SHADER_STORAGE_BUFFER;

			default:
				FE_LOG_CORE_ERROR("Unrecognized buffer usage");
				return GL_NONE;
			}
		}

		GLenum DepthTestTypeToGLEnum(Description::Pipeline::DepthTestType type)
		{
			switch (type)
			{
			case Description::Pipeline::DepthTestType::None:
				FE_CORE_ASSERT(false, "Not specified DepthTestType");
				return GL_NONE;
			case Description::Pipeline::DepthTestType::Never:			return GL_NEVER;
			case Description::Pipeline::DepthTestType::Always:			return GL_ALWAYS;
			case Description::Pipeline::DepthTestType::NotEqual:		return GL_NOTEQUAL;
			case Description::Pipeline::DepthTestType::Less:			return GL_LESS;
			case Description::Pipeline::DepthTestType::LessEqual:		return GL_LEQUAL;
			case Description::Pipeline::DepthTestType::Equal:			return GL_EQUAL;
			case Description::Pipeline::DepthTestType::GreaterEqual:	return GL_GEQUAL;
			case Description::Pipeline::DepthTestType::Greater:			return GL_GREATER;

			default:
				FE_LOG_CORE_ERROR("Unrecognized DepthTestType");
				return GL_NONE;
			}
		}

		GLenum BlendFunctionToGLEnum(Description::Pipeline::BlendFunction func)
		{
			switch (func)
			{
			case Description::Pipeline::BlendFunction::None:
				FE_CORE_ASSERT(false, "Not specified BlendFunction");
				return GL_NONE;
			case Description::Pipeline::BlendFunction::SourceAlpha:			return GL_SRC_ALPHA;
			case Description::Pipeline::BlendFunction::OneMinusSourceAlpha:	return GL_ONE_MINUS_SRC_ALPHA;

			default:
				FE_LOG_CORE_ERROR("Unrecognized BlendFunction");
				return GL_NONE;
			}
		}
	}
}