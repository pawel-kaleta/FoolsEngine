#pragma once

#include "FoolsEngine\Renderer\APIAbstraction\Shader.h"


namespace fe
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		virtual ~OpenGLShader();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual const std::string& GetName() const override { return m_Name; };
		virtual const uint32_t& GetProgramID() const override { return m_ProgramID; };

		virtual void UploadUniform(const Uniform& uniform, void* dataPointer, uint32_t count = 1, bool transpose = false) override;
	private:
		std::string m_Name;
		uint32_t m_ProgramID;

	};
}