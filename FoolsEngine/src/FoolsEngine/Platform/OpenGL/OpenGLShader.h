#pragma once

#include "FoolsEngine\Renderer\Shader.h"

namespace fe
{
	class OpenGLShader : Shader
	{
	public:
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		virtual ~OpenGLShader();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual const std::string& GetName() const override { return m_Name; };
		virtual const uint32_t& GetProgramID() const override { return m_ProgramID; };

	private:
		std::string m_Name;
		uint32_t m_ProgramID;
	};
}