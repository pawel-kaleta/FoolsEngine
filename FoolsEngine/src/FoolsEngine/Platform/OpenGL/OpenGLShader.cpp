#include "FE_pch.h"

#include "OpenGLShader.h"


namespace fe
{
    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
        : m_Name(name), m_ProgramID(0)
    {
        FE_PROFILER_FUNC();

		std::unordered_map<GLenum, std::string> shaderSources;

		shaderSources[GL_VERTEX_SHADER] = vertexSource;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSource;

		Compile(shaderSources);
    }

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& shaderSource)
		: m_Name(name), m_ProgramID(0)
	{
		FE_PROFILER_FUNC();

		auto shaderSources = PreProcess(shaderSource);
		Compile(shaderSources);
	}

	OpenGLShader::OpenGLShader(const std::string& filePath)
		: m_ProgramID(0)
	{
		FE_PROFILER_FUNC();

		m_Name = FileNameFromFilepath(filePath);

		std::string shaderSource = ReadFile(filePath);
		auto shaderSources = PreProcess(shaderSource);
		Compile(shaderSources);
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

	void OpenGLShader::BindTextureSlot(const ShaderTextureSlot& textureSlot, int32_t* rendererTextureSlot, uint32_t count)
	{

		GLint location = glGetUniformLocation(m_ProgramID, textureSlot.GetName().c_str());
		glUniform1iv(location, count, (GLint*)rendererTextureSlot);
	}

	void OpenGLShader::BindTextureSlot(const ShaderTextureSlot& textureSlot, int32_t rendererTextureSlot)
	{
		GLint location = glGetUniformLocation(m_ProgramID, textureSlot.GetName().c_str());
		glUniform1i(location, rendererTextureSlot);
	}

	std::string OpenGLShader::ReadFile(const std::string& filePath)
	{
		std::ifstream in(filePath, std::ios::in, std::ios::binary);

		if (!in.good())
		{
			FE_LOG_CORE_ERROR("Could not load shader file: \"{0}\"", filePath);
			return "";
		}

		std::string shaderSource;
		in.seekg(0, std::ios::end);
		shaderSource.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&shaderSource[0], shaderSource.size());
		in.close();

		return shaderSource;
	}

	GLenum OpenGLShader::ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;
		FE_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& shaderSource)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = shaderSource.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = shaderSource.find_first_of("\r\n", pos);
			FE_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = shaderSource.substr(begin, eol - begin);
			FE_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = shaderSource.find_first_not_of("\r\n", eol);
			FE_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = shaderSource.find(typeToken, nextLinePos);

			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? shaderSource.substr(nextLinePos) : shaderSource.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		FE_PROFILER_FUNC();


		FE_CORE_ASSERT(shaderSources.size() <= 2, "No support for more then 2 shaders!");

		std::array<GLuint, 2> shaders;
		int shadersCount = 0;

		for (auto& keyValue : shaderSources)
		{
			FE_PROFILER_SCOPE("Shader compilation");
			const GLenum type = keyValue.first;
			const std::string& source = keyValue.second;

			GLuint shader = glCreateShader(type);
			shaders[shadersCount++] = shader;

			const GLchar* sourceCStr = (const GLchar*)source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			{
				FE_PROFILER_SCOPE("OpenGL shader compilation");
				glCompileShader(shader);
			}

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				for (int i = 0; i < shadersCount; i++)
					glDeleteShader(shaders[i]);

				FE_LOG_CORE_ERROR("{0}", infoLog.data());
				FE_CORE_ASSERT(false, "OpenGL shader compilation failed!");

				return;
			}
		}

		GLuint programID = glCreateProgram();

		for (int i = 0; i < shadersCount; i++)
			glAttachShader(programID, shaders[i]);

		{
			FE_PROFILER_SCOPE("OpenGL Shader linking");
			glLinkProgram(programID);
		}

		GLint isLinked = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(programID);

			for (int i = 0; i < shadersCount; i++)
				glDeleteShader(shaders[i]);

			FE_LOG_CORE_ERROR("{0}", infoLog.data());
			FE_CORE_ASSERT(false, "OpenGL shader program linking failed!");
			return;
		}

		for (int i = 0; i < shadersCount; i++)
		{
			glDetachShader(programID, shaders[i]);
			glDeleteShader(shaders[i]);
		}

		m_ProgramID = programID;
	}

}