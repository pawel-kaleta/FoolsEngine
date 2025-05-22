#include "FE_pch.h"
#include "OpenGLShader.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"

#include <glad\glad.h>

namespace fe
{
    OpenGLShader::~OpenGLShader()
    {
        FE_PROFILER_FUNC();

		if (m_ProgramID)
			glDeleteProgram(m_ProgramID);
    }

    void OpenGLShader::Bind()
    {
        FE_PROFILER_FUNC();

		glUseProgram(m_ProgramID);
    }

    void OpenGLShader::Unbind()
    {
        FE_PROFILER_FUNC();

        glUseProgram(0);
    }

	void OpenGLShader::UploadUniform(const Uniform& uniform, const void* dataPointer, uint32_t count, bool transpose)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(!(transpose && (uniform.GetStructure() != ShaderData::Structure::Matrix)), "Only a matrix can be transposed!");
		
		if (!count)
		{
			FE_CORE_ASSERT(false, "Count must be positive");
			return;
		}

		auto& name = uniform.GetName();
		GLint location;

		auto x = m_UniformLocations.find(name);
		if (x == m_UniformLocations.end())
		{
			location = glGetUniformLocation(m_ProgramID, uniform.GetName().c_str());
			m_UniformLocations[name] = location;
		}
		else
			location = x->second;

		switch (uniform.GetType())
		{
		case ShaderData::Type::None:
			FE_CORE_ASSERT(false, "Unknown Shader Data Type of uniform!");
			return;
		case ShaderData::Type::Bool:
		case ShaderData::Type::Int:
			glUniform1iv(location, count, (GLint*)dataPointer);
			return;
		case ShaderData::Type::Bool2:
		case ShaderData::Type::Int2:
			glUniform2iv(location, count, (GLint*)dataPointer);
			return;
		case ShaderData::Type::Bool3:
		case ShaderData::Type::Int3:
			glUniform3iv(location, count, (GLint*)dataPointer);
			return;
		case ShaderData::Type::Bool4:
		case ShaderData::Type::Int4:
			glUniform4iv(location, count, (GLint*)dataPointer);
			return;
		case ShaderData::Type::UInt:
			glUniform1uiv(location, count, (GLuint*)dataPointer);
			return;
		case ShaderData::Type::UInt2:
			glUniform2uiv(location, count, (GLuint*)dataPointer);
			return;
		case ShaderData::Type::UInt3:
			glUniform3uiv(location, count, (GLuint*)dataPointer);
			return;
		case ShaderData::Type::UInt4:
			glUniform4uiv(location, count, (GLuint*)dataPointer);
			return;
		case ShaderData::Type::Float:
			glUniform1fv(location, count, (GLfloat*)dataPointer);
			return;
		case ShaderData::Type::Float2:
			glUniform2fv(location, count, (GLfloat*)dataPointer);
			return;
		case ShaderData::Type::Float3:
			glUniform3fv(location, count, (GLfloat*)dataPointer);
			return;
		case ShaderData::Type::Float4:
			glUniform4fv(location, count, (GLfloat*)dataPointer);
			return;
		case ShaderData::Type::Double:
		case ShaderData::Type::Double2:
		case ShaderData::Type::Double3:
		case ShaderData::Type::Double4:
			FE_CORE_ASSERT(false, "Double precision Shader Data Type of uniform not supported!");
			return;
		case ShaderData::Type::Mat2:
			glUniformMatrix2fv(location, count,	transpose, (GLfloat*) dataPointer);
			return;
		case ShaderData::Type::Mat2x3:
			glUniformMatrix2x3fv(location, count, transpose, (GLfloat*)dataPointer);
			return;
		case ShaderData::Type::Mat2x4:
			glUniformMatrix2x4fv(location, count, transpose, (GLfloat*)dataPointer);
			return;
		case ShaderData::Type::Mat3x2:
			glUniformMatrix3x2fv(location, count, transpose, (GLfloat*)dataPointer);
			return;
		case ShaderData::Type::Mat3:
			glUniformMatrix3fv(location, count, transpose, (GLfloat*)dataPointer);
			return;
		case ShaderData::Type::Mat3x4:
			glUniformMatrix3x4fv(location, count, transpose, (GLfloat*)dataPointer);
			return;
		case ShaderData::Type::Mat4x2:
			glUniformMatrix4x2fv(location, count, transpose, (GLfloat*)dataPointer);
			return;
		case ShaderData::Type::Mat4x3:
			glUniformMatrix4x3fv(location, count, transpose, (GLfloat*)dataPointer);
			return;
		case ShaderData::Type::Mat4:
			glUniformMatrix4fv(location, count, transpose, (GLfloat*)dataPointer);
			return;
		default:
			FE_CORE_ASSERT(false, "Unrecognised Shader Data Type of uniform!");
			return;
		}

	}

	void OpenGLShader::BindTextureSlot(const ShaderTextureSlot& textureSlot, RenderTextureSlotID* rendererTextureSlot, uint32_t count)
	{
		FE_CORE_ASSERT((size_t)rendererTextureSlot * count, "RenderTextureSlotID pointer is null or count is 0 - ptr_val: {0}; count: {1}", (size_t)rendererTextureSlot, count);

		auto& name = textureSlot.GetName();
		GLint location;

		auto x = m_UniformLocations.find(name);
		if (x == m_UniformLocations.end())
		{
			location = glGetUniformLocation(m_ProgramID, name.c_str());
			m_UniformLocations[name] = location;
		}
		else
			location = x->second;

		glUniform1iv(location, count, (GLint*)rendererTextureSlot);
	}

	void OpenGLShader::BindTextureSlot(const ShaderTextureSlot& textureSlot, RenderTextureSlotID rendererTextureSlot)
	{
		auto& name = textureSlot.GetName();
		GLint location;

		auto x = m_UniformLocations.find(name);
		if (x == m_UniformLocations.end())
		{
			location = glGetUniformLocation(m_ProgramID, name.c_str());
			m_UniformLocations[name] = location;
		}
		else
			location = x->second;

		glUniform1iv(location, 1, (GLint*) & rendererTextureSlot);
	}
}