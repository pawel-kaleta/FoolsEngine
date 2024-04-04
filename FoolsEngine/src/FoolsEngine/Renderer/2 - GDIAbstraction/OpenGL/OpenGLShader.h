#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"

#include <glad\glad.h>

namespace fe
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const AssetSignature& assetSignature, const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		OpenGLShader(const AssetSignature& assetSignature, const std::string& name, const std::string& shaderSource);
		OpenGLShader(const AssetSignature& assetSignature, const std::string& filepath);
		virtual ~OpenGLShader() override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual const std::string& GetName() const override { return m_Name; };
		virtual const uint32_t& GetProgramID() const override { return m_ProgramID; };

		virtual void UploadUniform(const Uniform& uniform, void* dataPointer, uint32_t count = 1, bool transpose = false) override;
		virtual void BindTextureSlot(const ShaderTextureSlot& textureSlot, uint32_t* rendererTextureSlot, uint32_t count) override;
		virtual void BindTextureSlot(const ShaderTextureSlot& textureSlot, uint32_t rendererTextureSlot) override;
	private:
		uint32_t m_ProgramID;
		std::string m_Name;

		static std::string ReadFile(const std::string& filePath);
		static GLenum ShaderTypeFromString(const std::string& type);
		static std::unordered_map<GLenum, std::string> PreProcess(const std::string& shaderSource);

		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	};
}