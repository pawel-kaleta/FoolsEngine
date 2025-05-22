#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"

typedef unsigned int GLuint;
typedef int GLint;

namespace fe
{
	class ShaderTextureSlot;

	class OpenGLShader final : public AssetComponent
	{
	public:
		OpenGLShader(GLuint programID) : m_ProgramID(programID) { };
		OpenGLShader() = default;
		~OpenGLShader();

		void Bind();
		void Unbind();

		const GLuint& GetProgramID() const { return m_ProgramID; };

		void UploadUniform(const Uniform& uniform, const void* dataPointer, uint32_t count = 1, bool transpose = false);
		void BindTextureSlot(const ShaderTextureSlot& textureSlot, RenderTextureSlotID* rendererTextureSlot, uint32_t count);
		void BindTextureSlot(const ShaderTextureSlot& textureSlot, RenderTextureSlotID rendererTextureSlot);
	private:
		GLuint m_ProgramID = 0;
		std::unordered_map<std::string, GLint> m_UniformLocations;
	};
}