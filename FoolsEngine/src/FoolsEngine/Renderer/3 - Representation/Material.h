#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"

#include "FoolsEngine\Assets\AssetHandle.h"

namespace fe
{
	struct ACMaterialData final : public AssetComponent
	{
		AssetID ShaderID;
		std::vector<Uniform> Uniforms;
		std::vector<ShaderTextureSlot> TextureSlots;

		void Init();
	};

	class Material : public Asset
	{
	public:
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::MaterialAsset; }

		AssetID GetShaderID()               const { return Get<ACMaterialData>().ShaderID; }
		void    SetShader(AssetID shaderID)       { Get<ACMaterialData>().ShaderID = shaderID; }

		      std::vector<Uniform>& GetUniforms()       { return Get<ACMaterialData>().Uniforms; }
		const std::vector<Uniform>& GetUniforms() const { return Get<ACMaterialData>().Uniforms; }

		      std::vector<ShaderTextureSlot>& GetTextureSlots()       { return Get<ACMaterialData>().TextureSlots; }
		const std::vector<ShaderTextureSlot>& GetTextureSlots() const { return Get<ACMaterialData>().TextureSlots; }

		virtual void PlaceCoreComponent() final override { Emplace<ACMaterialData>().Init(); }
		virtual void Release() final override { };

		void MakeMaterial(
			AssetID shaderID,
			const std::initializer_list<Uniform>& uniforms,
			const std::initializer_list<ShaderTextureSlot>& textureSlots);

	protected:
		Material(ECS_AssetHandle ECS_handle) : Asset(ECS_handle) {}


		friend class AssetManager;
		void Init() {};
	};

	

	
}