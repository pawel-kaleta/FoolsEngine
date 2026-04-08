#pragma once

#include "FoolsEngine/Renderer/1 - Description/ShaderInterface.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"

#include <glad/glad.h>

namespace fe::Resource
{
	using namespace Description::ShaderInterface;

	struct ShaderBase
	{
		UInt SpecificationID;

		virtual void Create(String source) = 0;
		virtual void Destroy() = 0;
	};

	template <GAPIType::ValueType GAPI>
	struct RShader;

	template <>
	struct RShader<GAPIType::OpenGL> final : public ShaderBase
	{
		GLuint ShaderOpenGLID;

		virtual void Create(String source) override;
		virtual void Destroy() override;
	};

	using RShader_OpenGL = RShader<GAPIType::OpenGL>;
}