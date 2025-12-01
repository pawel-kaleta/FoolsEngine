#pragma once

#include <filesystem>
#include "FoolsEngine\Assets\Loaders\LoaderType.h"
#include "FoolsEngine\Assets\AssetHandle.h"
#include "FoolsEngine\Renderer\2 - GAPIAbstraction\Shader.h"

namespace fe
{
	class ShaderLoader
	{
	public:
		static constexpr LoaderType GetTypeStatic() { return LoaderType::Shader; }
		static constexpr AssetType GetAssetTypeStatic() { return AssetType::Shader; }
		static void LoadShader(const std::filesystem::path& filePath, const AssetUser<Shader>& shaderUser);
		static void LoadShader(const AssetUser<Shader>& shaderUser)
		{
			auto& path = shaderUser.GetFilepath();
			LoadShader(path, shaderUser);
		}
		static void CompileShader(GAPIType GAPI, const AssetUser<Shader>& shaderUser);

		static bool IsKnownExtension(const std::pmr::string& extension);
		static bool IsKnownAssetType(AssetType assetType);

		static const char* GetExtensionAlias() { return "Shader Source"; }
	private:
		static void PreProcess(const AssetUser<Shader>& shaderUser);

		static void GLCompileShader(const AssetUser<Shader>& shaderUser);
	};
}