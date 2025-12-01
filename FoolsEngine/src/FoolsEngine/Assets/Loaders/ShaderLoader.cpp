#include "FE_pch.h"

#include "ShaderLoader.h"
#include "FoolsEngine\Renderer\2 - GAPIAbstraction\OpenGL\OpenGLShader.h"
#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"

#include "FoolsEngine\Memory\Scratchpad.h"

#include <glad\glad.h>

namespace fe
{
	void ShaderLoader::LoadShader(const std::filesystem::path& filePath, const AssetUser<Shader>& shaderUser)
	{
		FE_PROFILER_FUNC();


		std::ifstream in(filePath, std::ios::in, std::ios::binary);

		if (!in.good())
		{
			Scratchpad sp;
			FE_LOG_CORE_ERROR("Could not load shader file: \"{0}\"", filePath.string<PMR_STRING_TEMPLATE_PARAMS>(&sp));
			return;
		}

		auto& shader_core = shaderUser.GetCoreComponent();
		auto& shader_source = shader_core.ShaderSource;
		
		in.seekg(0, std::ios::end);
		shader_source.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&shader_source[0], shader_source.size());
		in.close();

		return;
	}

	void ShaderLoader::CompileShader(GAPIType GAPI, const AssetUser<Shader>& shaderUser)
	{
		switch (GAPI.Value)
		{
		case GAPIType::None:
			FE_CORE_ASSERT(false, "Unspecified GAPIType");
			return;

		case GAPIType::OpenGL:
			return GLCompileShader(shaderUser);
		}
	}

	bool ShaderLoader::IsKnownExtension(const std::pmr::string& extension)
	{
		static const char* s_known_extensions[] = {
			".glsl"
		};

		for (auto& known_extension : s_known_extensions)
		{
			if (extension == known_extension)
			{
				return true;
			}
		}

		return false;
	}

	bool ShaderLoader::IsKnownAssetType(AssetType assetType)
	{
		static const AssetType s_known_types[] = {
			AssetType::Shader
		};

		for (const auto& known_type : s_known_types)
		{
			if (known_type == assetType)
			{
				return true;
			}
		}

		return false;
	}

	void ShaderLoader::GLCompileShader(const AssetUser<Shader>& shaderUser)
	{
		FE_PROFILER_FUNC();

		auto& shader_core = shaderUser.GetCoreComponent();

		if (!shader_core.ShaderSource.empty())
			PreProcess(shaderUser);

		std::unordered_map<GLenum, const std::string*> shader_sources;
		shader_sources[GL_VERTEX_SHADER] = &(shader_core.VertexSource);
		shader_sources[GL_FRAGMENT_SHADER] = &(shader_core.FragmentSource);

		std::array<GLuint, 2> shaders;
		int shaders_count = 0;

		for (auto& key_value : shader_sources)
		{
			FE_PROFILER_SCOPE("Shader compilation");
			const GLenum type = key_value.first;
			const std::string* source = key_value.second;

			GLuint shader = glCreateShader(type);
			shaders[shaders_count++] = shader;

			const GLchar* source_c_str = (const GLchar*)source->c_str();
			glShaderSource(shader, 1, &source_c_str, 0);

			GLint compilation_success;
			{
				FE_PROFILER_SCOPE("OpenGL shader compilation");
				glCompileShader(shader);
				glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_success);
			}

			if (compilation_success == GL_FALSE)
			{
				GLint log_length = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

				std::vector<GLchar> info_log(log_length);
				glGetShaderInfoLog(shader, log_length, &log_length, info_log.data());

				for (int i = 0; i < shaders_count; i++)
					glDeleteShader(shaders[i]);

				FE_LOG_CORE_ERROR("{0}", info_log.data());
				FE_CORE_ASSERT(false, "OpenGL shader compilation failed!");

				return;
			}
		}

		GLuint program_ID = glCreateProgram();

		for (int i = 0; i < shaders_count; i++)
			glAttachShader(program_ID, shaders[i]);

		GLint linking_success = 0;
		{
			FE_PROFILER_SCOPE("OpenGL Shader linking");
			glLinkProgram(program_ID);
			glGetProgramiv(program_ID, GL_LINK_STATUS, (int*)&linking_success);
		}

		if (linking_success == GL_FALSE)
		{
			GLint log_length = 0;
			glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &log_length);

			std::vector<GLchar> info_log(log_length);
			glGetProgramInfoLog(program_ID, log_length, &log_length, info_log.data());

			glDeleteProgram(program_ID);

			for (int i = 0; i < shaders_count; i++)
				glDeleteShader(shaders[i]);

			FE_LOG_CORE_ERROR("{0}", info_log.data());
			FE_CORE_ASSERT(false, "OpenGL shader program linking failed!");
			return;
		}

		for (int i = 0; i < shaders_count; i++)
		{
			glDetachShader(program_ID, shaders[i]);
			glDeleteShader(shaders[i]);
		}

		shaderUser.CreateGAPIShader<OpenGLShader>(program_ID);
	}

	void ShaderLoader::PreProcess(const AssetUser<Shader>& shaderUser)
	{
		//TO DO: stop this nonsens with multiple shaders in one file

		auto& shader_core = shaderUser.GetCoreComponent();
		auto& shader_source = shader_core.ShaderSource;

		const char* type_token = "#type";
		size_t type_token_length = strlen(type_token);
		size_t pos = shader_source.find(type_token, 0);
		while (pos != std::string::npos)
		{
			size_t eol = shader_source.find_first_of("\r\n", pos);
			FE_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + type_token_length + 1;
			std::string type = shader_source.substr(begin, eol - begin);

			std::string* data_location = nullptr;

			if (type == "vertex")
				data_location = &(shader_core.VertexSource);
			if (type == "fragment" || type == "pixel")
				data_location = &(shader_core.FragmentSource);

			size_t next_line_pos = shader_source.find_first_not_of("\r\n", eol);
			FE_CORE_ASSERT(next_line_pos != std::string::npos, "Syntax error");
			pos = shader_source.find(type_token, next_line_pos);

			*data_location = (pos == std::string::npos) ? shader_source.substr(next_line_pos) : shader_source.substr(next_line_pos, pos - next_line_pos);
		}

		shader_source.clear();
	}
}