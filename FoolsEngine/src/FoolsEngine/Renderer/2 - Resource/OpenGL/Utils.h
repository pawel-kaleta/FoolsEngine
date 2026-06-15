#pragma once

#include "FoolsEngine/Renderer/1 - Description/Data.h"
#include "FoolsEngine/Renderer/1 - Description/Texture.h"
#include "FoolsEngine/Renderer/1 - Description/Pipeline.h"
#include "FoolsEngine/Renderer/1 - Description/ShaderInterface.h"

#include <glad/glad.h>

namespace fe::Resource
{
	namespace Utils
	{
		GLenum DataPrimitiveToGLBaseType(Description::Data::Primitive primitive);
		GLenum FormatToGLFormat(Description::Texture::Format format);
		GLenum FormatToGLInternalFormat(Description::Texture::Format format);
		GLenum FormatToGLType(Description::Texture::Format format);
		GLenum ShaderTypeToGLEnum(Description::ShaderInterface::ShaderType type);
		GLenum DepthTestTypeToGLEnum(Description::Pipeline::DepthTestType type);
		GLenum BlendFunctionToGLEnum(Description::Pipeline::BlendFunction func);
	}
}