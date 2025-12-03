#pragma once

#include "Shader.h"

#include "FoolsEngine\Renderer\1 - Description\ShaderInterface.h"

#include <glad\glad.h>

namespace fe::Resource
{
	struct ProgramBase
	{
		uint32_t m_SpecificationID;
		std::vector<ShaderBase*> m_Shaders;

		virtual void Create() = 0;
		virtual void Destroy() = 0;

		virtual void BindTextureSamplerToRendererTextureSlot(const std::pmr::string& samplerName, RenderTextureSlotID rendererTextureSlot) const = 0;
		virtual void BindTextureSamplerToRendererTextureSlot(size_t samplerIndex, RenderTextureSlotID rendererTextureSlot) const = 0;
		virtual void UploadUniform(const std::pmr::string& uniformName, const void* data) const = 0;
		virtual void UploadUniform(size_t uniformIndex, const void* data) const = 0;
	};

	struct Program_OpenGL final : ProgramBase
	{
		GLuint m_OpenGLID;

		struct
		{
			std::vector<GLint> MainUniforms;
			std::vector<GLint> TextureSamplers;
		} m_BindingLocations;

		virtual void Create() override;
		virtual void Destroy() override;
		virtual void BindTextureSamplerToRendererTextureSlot(const std::pmr::string& samplerName, RenderTextureSlotID rendererTextureSlot) const override;
		virtual void BindTextureSamplerToRendererTextureSlot(size_t samplerIndex, RenderTextureSlotID rendererTextureSlot) const override;
		virtual void UploadUniform(const std::pmr::string& uniformName, const void* data) const override;
		virtual void UploadUniform(size_t uniformIndex, const void* data) const override;
		void UploadUniform(Description::Data::Type type, uint32_t count, GLint location, const void* data) const;
	};
}