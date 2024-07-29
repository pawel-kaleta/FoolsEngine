#include "FE_pch.h"
#include "Material.h"

namespace fe
{
	void Material::MakeMaterial(AssetUser<Material>& materialUser, AssetHandle<Shader> shader, const std::initializer_list<Uniform>& uniforms, const std::initializer_list<ShaderTextureSlot>& textureSlots)
	{
		materialUser.Emplace<ACShader>().Shader = shader;
		materialUser.Emplace<ACUniforms>().Uniforms = uniforms;
		materialUser.Emplace<ACShaderTextureSlots>().TextureSlots = textureSlots;

		return;
	}
}