#include "FE_pch.h"
#include "Material.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"

namespace fe
{
	void Material::PlaceCoreComponents()
	{
		Emplace<ACShader>().ShaderID = NullAssetID;
		Emplace<ACUniforms>();
		Emplace<ACShaderTextureSlots>();
	}


	void Material::MakeMaterial(AssetID shaderID, const std::initializer_list<Uniform>& uniforms, const std::initializer_list<ShaderTextureSlot>& textureSlots)
	{
		Get<ACShader>().ShaderID = shaderID;
		Get<ACUniforms>().Uniforms = uniforms;
		Get<ACShaderTextureSlots>().TextureSlots = textureSlots;
	}
}