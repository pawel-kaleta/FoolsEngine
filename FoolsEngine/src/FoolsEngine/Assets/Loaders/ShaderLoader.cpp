#include "FE_pch.h"

#include "ShaderLoader.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\OpenGL\OpenGLShader.h"
#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"

#include <glad\glad.h>

namespace fe
{
	void ShaderLoader::LoadShader(const std::filesystem::path& filePath, AssetUser<Shader>& shaderUser)
	{
		std::ifstream in(filePath, std::ios::in, std::ios::binary);

		if (!in.good())
		{
			FE_LOG_CORE_ERROR("Could not load shader file: \"{0}\"", filePath);
			return;
		}

		auto& shader_core = shaderUser.GetCoreComponent();
		auto& shaderSource = shader_core.ShaderSource;
		
		in.seekg(0, std::ios::end);
		shaderSource.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&shaderSource[0], shaderSource.size());
		in.close();

		return;
	}

	void ShaderLoader::CompileShader(GDIType GDI, AssetUser<Shader>& shaderUser)
	{
		switch (GDI.Value)
		{
		case GDIType::None:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			return;

		case GDIType::OpenGL:
			return GLCompileShader(shaderUser);
		}
	}

	bool ShaderLoader::IsKnownExtension(const std::pmr::string& extension)
	{
		static std::pmr::string knownExtensions[] = {
			".glsl"
		};

		for (auto& knownExtension : knownExtensions)
		{
			if (extension == knownExtension)
			{
				return true;
			}
		}

		return false;
	}

	void ShaderLoader::GLCompileShader(AssetUser<Shader>& shaderUser)
	{
		FE_PROFILER_FUNC();

		auto shader_core = shaderUser.GetCoreComponent();

		if (!shader_core.ShaderSource.empty())
			PreProcess(shaderUser);

		std::unordered_map<GLenum, const std::string*> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = &(shader_core.VertexSource);
		shaderSources[GL_FRAGMENT_SHADER] = &(shader_core.FragmentSource);

		std::array<GLuint, 2> shaders;
		int shadersCount = 0;

		for (auto& keyValue : shaderSources)
		{
			FE_PROFILER_SCOPE("Shader compilation");
			const GLenum type = keyValue.first;
			const std::string* source = keyValue.second;

			GLuint shader = glCreateShader(type);
			shaders[shadersCount++] = shader;

			const GLchar* sourceCStr = (const GLchar*)source->c_str();
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

		FE_LOG_CORE_DEBUG("{0}", programID);

		shaderUser.CreateGDIShader<OpenGLShader>(programID);
	}

	void ShaderLoader::PreProcess(AssetUser<Shader>& shaderUser)
	{
		auto shader_core = shaderUser.GetCoreComponent();
		auto& shaderSource = shader_core.ShaderSource;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = shaderSource.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = shaderSource.find_first_of("\r\n", pos);
			FE_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = shaderSource.substr(begin, eol - begin);

			std::string* dataLocation = nullptr;

			if (type == "vertex")
				dataLocation = &(shader_core.VertexSource);
			if (type == "fragment" || type == "pixel")
				dataLocation = &(shader_core.FragmentSource);

			size_t nextLinePos = shaderSource.find_first_not_of("\r\n", eol);
			FE_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = shaderSource.find(typeToken, nextLinePos);

			*dataLocation = (pos == std::string::npos) ? shaderSource.substr(nextLinePos) : shaderSource.substr(nextLinePos, pos - nextLinePos);
		}

		shaderSource.clear();
	}
}