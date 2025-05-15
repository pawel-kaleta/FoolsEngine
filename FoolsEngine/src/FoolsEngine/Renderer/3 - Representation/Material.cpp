#include "FE_pch.h"
#include "Material.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"

namespace fe
{
	void ACMaterialData::Init()
	{
		ShaderID = 0;
		Uniforms.clear();
		TextureSlots.clear();
	}
	
	void Material::MakeMaterial(AssetID shaderID, const std::initializer_list<Uniform>& uniforms, const std::initializer_list<ShaderTextureSlot>& textureSlots)
	{
		auto& ACData = Get<ACMaterialData>();
		ACData.ShaderID = shaderID;
		ACData.Uniforms = uniforms;
		ACData.TextureSlots = textureSlots;
	}
}