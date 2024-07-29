#pragma once

#include <filesystem>
#include "FoolsEngine\Assets\AssetHandle.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"

namespace fe
{
	class ShaderLoader
	{
	public:
		static void LoadShader(const std::filesystem::path& filePath, AssetUser<Shader>& shaderUser);
		static void CompileShader(GDIType GDI, AssetUser<Shader>& shaderUser);
	private:
		static void PreProcess(AssetUser<Shader>& shaderUser);

		static void GLCompileShader(AssetUser<Shader>& shaderUser);
	};
}