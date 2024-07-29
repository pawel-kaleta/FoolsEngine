#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Core\UUID.h"

#include "FoolsEngine\Assets\AssetHandle.h"

namespace fe
{
	struct ACShader final : public AssetComponent
	{
		AssetHandle<Shader> Shader;
	};

	struct ACUniforms final : public AssetComponent
	{
		std::vector<Uniform> Uniforms;
	};

	struct ACShaderTextureSlots final : public AssetComponent
	{
		std::vector<ShaderTextureSlot> TextureSlots;
	};

	class Material : public Asset
	{
	public:
		virtual AssetType GetType() { return GetTypeStatic(); }
		static AssetType GetTypeStatic() { return AssetType::MaterialAsset; }

		virtual void UnloadFromGPU() override final { FE_CORE_ASSERT(false, "Not implemented"); };
		virtual void UnloadFromCPU() override final { FE_CORE_ASSERT(false, "Not implemented"); };

		AssetHandle<Shader> GetShader() const { return Get<ACShader>().Shader; }
		void SetShader(AssetHandle<Shader> shader) { Get<ACShader>().Shader = shader; }

		const std::vector<Uniform>& GetUniforms() const { return Get<ACUniforms>().Uniforms; }
		      std::vector<Uniform>& GetUniforms()       { return Get<ACUniforms>().Uniforms; }

		const std::vector<ShaderTextureSlot>& GetTextureSlots() const { return Get<ACShaderTextureSlots>().TextureSlots; }
		      std::vector<ShaderTextureSlot>& GetTextureSlots()       { return Get<ACShaderTextureSlots>().TextureSlots; }

		static void MakeMaterial(
			AssetUser<Material>& materialUser,
			AssetHandle<Shader> shader,
			const std::initializer_list<Uniform>& uniforms,
			const std::initializer_list<ShaderTextureSlot>& textureSlots);

	protected:
		Material(ECS_AssetHandle ECS_handle) : Asset(ECS_handle) {}
	};

	

	
}