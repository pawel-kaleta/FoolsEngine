#include "FE_pch.h"

#include "FoolsEngine\Renderer\2 - Resource\Program.h"
#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include "FoolsEngine\Memory\Scratchpad.h"

namespace fe::Resource
{
	void Program_OpenGL::Create()
	{
		FE_PROFILER_FUNC();

		GLuint m_OpenGLID = glCreateProgram();

		std::vector<Shader_OpenGL*>& shaders = *(std::vector<Shader_OpenGL*>*) & m_Shaders;

		for (int i = 0; i < shaders.size(); i++)
			glAttachShader(m_OpenGLID, shaders[i]->m_OpenGLID);

		GLint linking_success = 0;
		{
			FE_PROFILER_SCOPE("OpenGL Shader linking");
			glLinkProgram(m_OpenGLID);
			glGetProgramiv(m_OpenGLID, GL_LINK_STATUS, (int*)&linking_success);
		}

		if (linking_success == GL_FALSE)
		{
			GLint log_length = 0;
			glGetProgramiv(m_OpenGLID, GL_INFO_LOG_LENGTH, &log_length);

			Scratchpad sp;
			std::pmr::vector<GLchar> info_log(log_length, &sp);
			glGetProgramInfoLog(m_OpenGLID, log_length, &log_length, info_log.data());

			glDeleteProgram(m_OpenGLID);
			m_OpenGLID = -1;

			FE_LOG_CORE_ERROR("{0}", info_log.data());
			FE_CORE_ASSERT(false, "OpenGL shader program linking failed!");
			return;
		}

		for (int i = 0; i < shaders.size(); i++)
		{
			glDetachShader(m_OpenGLID, shaders[i]->m_OpenGLID); // do we need this?
		}

		const auto& spec = Description::Library::Get().ProgramSpecs[m_SpecificationID];

		const auto& uniforms = Description::Library::Get().BufferLayouts[spec.MainUniformsLayoutID];

		for (const auto& uniform : uniforms.Elements)
		{
			GLint location = glGetUniformLocation(m_OpenGLID, uniform.Name.c_str());
			m_BindingLocations.MainUniforms.push_back(location);
		}

		for (const auto& texture_sampler : spec.TextureSamplers)
		{
			GLint location = glGetUniformLocation(m_OpenGLID, texture_sampler.Name.c_str());
			m_BindingLocations.TextureSamplers.push_back(location);
		}
	}

	void Program_OpenGL::Destroy()
	{
		FE_PROFILER_FUNC();

		glDeleteProgram(m_OpenGLID);
	}

	void Program_OpenGL::BindTextureSamplerToRendererTextureSlot(const std::pmr::string& samplerName, RenderTextureSlotID rendererTextureSlot) const
	{
		const auto& spec = Description::Library::Get().ProgramSpecs[m_SpecificationID];

		for (size_t i = 0; i < spec.TextureSamplers.size(); ++i)
		{
			const auto& sampler = spec.TextureSamplers[i];

			if (sampler.Name.compare(samplerName))
			{
				BindTextureSamplerToRendererTextureSlot(i, rendererTextureSlot);
				return;
			}
		}
	}

	void Program_OpenGL::BindTextureSamplerToRendererTextureSlot(size_t samplerIndex, RenderTextureSlotID rendererTextureSlot) const
	{
		GLuint texture_unit = rendererTextureSlot;
		glUniform1ui(m_BindingLocations.TextureSamplers[samplerIndex], texture_unit);
	}

	void Program_OpenGL::UploadUniform(Description::Data::Type type, uint32_t count, GLint location, const void* data) const
	{
		if (!count)
		{
			FE_CORE_ASSERT(false, "Count must be positive");
			return;
		}

		switch (type)
		{
		case Description::Data::Type::None:
			FE_CORE_ASSERT(false, "Unknown Shader Data Type of uniform!");
			return;
		case Description::Data::Type::Bool:
		case Description::Data::Type::Int:
			glUniform1iv(location, count, (GLint*)data);
			return;
		case Description::Data::Type::Bool2:
		case Description::Data::Type::Int2:
			glUniform2iv(location, count, (GLint*)data);
			return;
		case Description::Data::Type::Bool3:
		case Description::Data::Type::Int3:
			glUniform3iv(location, count, (GLint*)data);
			return;
		case Description::Data::Type::Bool4:
		case Description::Data::Type::Int4:
			glUniform4iv(location, count, (GLint*)data);
			return;
		case Description::Data::Type::UInt:
			glUniform1uiv(location, count, (GLuint*)data);
			return;
		case Description::Data::Type::UInt2:
			glUniform2uiv(location, count, (GLuint*)data);
			return;
		case Description::Data::Type::UInt3:
			glUniform3uiv(location, count, (GLuint*)data);
			return;
		case Description::Data::Type::UInt4:
			glUniform4uiv(location, count, (GLuint*)data);
			return;
		case Description::Data::Type::Float:
			glUniform1fv(location, count, (GLfloat*)data);
			return;
		case Description::Data::Type::Float2:
			glUniform2fv(location, count, (GLfloat*)data);
			return;
		case Description::Data::Type::Float3:
			glUniform3fv(location, count, (GLfloat*)data);
			return;
		case Description::Data::Type::Float4:
			glUniform4fv(location, count, (GLfloat*)data);
			return;
		case Description::Data::Type::Double:
		case Description::Data::Type::Double2:
		case Description::Data::Type::Double3:
		case Description::Data::Type::Double4:
			FE_CORE_ASSERT(false, "Double precision Shader Data Type of uniform not supported!");
			return;
		case Description::Data::Type::Mat2:
			glUniformMatrix2fv(location, count, false, (GLfloat*)data);
			return;
		case Description::Data::Type::Mat2x3:
			glUniformMatrix2x3fv(location, count, false, (GLfloat*)data);
			return;
		case Description::Data::Type::Mat2x4:
			glUniformMatrix2x4fv(location, count, false, (GLfloat*)data);
			return;
		case Description::Data::Type::Mat3x2:
			glUniformMatrix3x2fv(location, count, false, (GLfloat*)data);
			return;
		case Description::Data::Type::Mat3:
			glUniformMatrix3fv(location, count, false, (GLfloat*)data);
			return;
		case Description::Data::Type::Mat3x4:
			glUniformMatrix3x4fv(location, count, false, (GLfloat*)data);
			return;
		case Description::Data::Type::Mat4x2:
			glUniformMatrix4x2fv(location, count, false, (GLfloat*)data);
			return;
		case Description::Data::Type::Mat4x3:
			glUniformMatrix4x3fv(location, count, false, (GLfloat*)data);
			return;
		case Description::Data::Type::Mat4:
			glUniformMatrix4fv(location, count, false, (GLfloat*)data);
			return;
		default:
			FE_CORE_ASSERT(false, "Unrecognised Shader Data Type of uniform!");
			return;
		}
	}

	void Program_OpenGL::UploadUniform(const std::pmr::string& uniformName, const void* data) const
	{
		const auto& spec = Description::Library::Get().ProgramSpecs[m_SpecificationID];

		const auto& uniforms = Description::Library::Get().BufferLayouts[spec.MainUniformsLayoutID];

		for (size_t i = 0; i < uniforms.Elements.size(); ++i)
		{
			const auto& uniform = uniforms.Elements[i];

			if (uniform.Name.compare(uniformName))
			{
				Description::Data::Type type = uniform.Type;
				uint32_t count = uniform.Count;
				GLint location = m_BindingLocations.MainUniforms[i];

				UploadUniform(type, count, location, data);

				return;
			}
		}

	}
	
	void Program_OpenGL::UploadUniform(size_t uniformIndex, const void* data) const
	{
		const auto& spec = Description::Library::Get().ProgramSpecs[m_SpecificationID];
		const auto& uniforms = Description::Library::Get().BufferLayouts[spec.MainUniformsLayoutID];
		const auto& uniform = uniforms.Elements[uniformIndex];

		Description::Data::Type type = uniform.Type;
		uint32_t count = uniform.Count;
		GLint location = m_BindingLocations.MainUniforms[uniformIndex];

		UploadUniform(type, count, location, data);
	}
}