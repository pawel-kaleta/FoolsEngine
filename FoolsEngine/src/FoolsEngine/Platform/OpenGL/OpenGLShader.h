#pragma once

#include "FoolsEngine\Renderer\APIAbstraction\Shader.h"


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

		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) override;
		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& values) override;

		virtual void UploadUniform(const std::string& name, void* dataPointer, SDType type, uint32_t count = 1, bool transpose = false) override;
	private:
		std::string m_Name;
		uint32_t m_ProgramID;

	};
}