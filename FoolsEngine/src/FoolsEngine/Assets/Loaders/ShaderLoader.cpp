#include "ShaderLoader.h"

#include "FoolsEngine/Foundation/Utils/Core.h"
#include "FoolsEngine/Foundation/Context/Pile.h"
#include "FoolsEngine/Foundation/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Containers/StringBuilder.h"

#include "FoolsEngine/Foundation/Debug/Logger.h"

#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Renderer/2 - Representation/Shader.h"

#include <glad/gl.h>

namespace fe
{
	void ShaderLoader::LoadShader(const std::filesystem::path& filePath, const AssetUser& shaderUser)
	{
		FE_PROFILER_FUNC();

		Pile p;
		STD_PMR_Allocator pmr(&p);
		
		std::ifstream in(filePath, std::ios::in, std::ios::binary);

		if (!in.good())
		{	
			Context::Log::Core.Error("Could not load shader file: \"{0}\""_FE, filePath.string<PMR_STRING_TEMPLATE_PARAMS>(&pmr));
			return;
		}

		auto& shader_core = shaderUser.Get<Shader::Core>();

		in.seekg(0, std::ios::end);
		CString shader_source = MakeCString(in.tellg(), &p);
		std::memset(shader_source.Data, '\0', shader_source.CountWithNull);
		in.seekg(0, std::ios::beg);
		in.read(shader_source.Data, shader_source.CountWithNull);
		in.close();

		auto& shader_component = shaderUser.Emplace<ACShader_GID<Render::GAPI::Platform::OpenGL>>();
		shader_component.mShaderGID = Render::GAPI::CreateShader(shader_core.mType, shader_source);

		return;
	}

	bool ShaderLoader::IsKnownExtension(String extension)
	{
		static const String s_known_extensions[] = {
			String(".glsl"_FE)
		};

		for (auto& known_extension : s_known_extensions)
		{
			if (CompareStringsEqual(known_extension, extension))
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