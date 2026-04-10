#pragma once

#include "FoolsEngine/Renderer/1 - Description/ShaderInterface.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"

#include <glad/glad.h>

namespace fe::Resource
{
	using namespace Description::ShaderInterface;

	struct RShader
	{
		UInt SpecificationID;

		virtual void Create(String source) = 0;
		virtual void Destroy() = 0;
	};



	struct RShader_OpenGL final : public RShader
	{
		GLuint OpenGLID;

		virtual void Create(String source) override;
		virtual void Destroy() override;
	};
}