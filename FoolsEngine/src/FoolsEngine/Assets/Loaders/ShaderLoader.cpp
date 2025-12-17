#include "FE_pch.h"

#include "ShaderLoader.h"
#include "FoolsEngine\Renderer\4 - Representation\Shader.h"
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
}