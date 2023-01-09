#include "FE_pch.h"
#include "OpenGLShader.h"

#include <glad\glad.h>

namespace fe
{
    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
        : m_Name(name)
    {
        FE_PROFILER_FUNC();

		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = (const GLchar*)vertexSource.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		{
			FE_PROFILER_SCOPE("OpenGL vertexShader compilation");
			glCompileShader(vertexShader);
		}

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			FE_LOG_CORE_ERROR("{0}", infoLog.data());
			FE_CORE_ASSERT(false, "OpenGL vertexShader compilation failed!");

			m_ProgramID = 0;
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar*)fragmentSource.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		{
			FE_PROFILER_SCOPE("OpenGL fragmentShader compilation");
			glCompileShader(fragmentShader);
		}

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			FE_LOG_CORE_ERROR("{0}", infoLog.data());
			FE_CORE_ASSERT(false, "OpenGL fragmentShader compilation failed!");

			m_ProgramID = 0;
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_ProgramID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_ProgramID, vertexShader);
		glAttachShader(m_ProgramID, fragmentShader);

		// Link our program
		{
			FE_PROFILER_SCOPE("OpenGL Shader linking");
			glLinkProgram(m_ProgramID);
		}

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_ProgramID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{

			GLint maxLength = 0;
			glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_ProgramID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_ProgramID);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			FE_LOG_CORE_ERROR("{0}", infoLog.data());
			FE_CORE_ASSERT(false, "OpenGL shader program linking failed!");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_ProgramID, vertexShader);
		glDetachShader(m_ProgramID, fragmentShader);
    }

    OpenGLShader::~OpenGLShader()
    {
        FE_PROFILER_FUNC();

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

	void OpenGLShader::UploadUniform(const Uniform& uniform, void* dataPointer, uint32_t count, bool transpose)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(!(transpose && (uniform.GetStructure() != ShaderData::Structure::Matrix)), "Only a matrix can be transposed!");
		
		if (count <= 0)
		{
			FE_CORE_ASSERT(false, "Count must be positive");
			return;
		}

		GLint location = glGetUniformLocation(m_ProgramID, uniform.GetName().c_str());

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
}