#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"

#include "FoolsEngine\Assets\AssetHandle.h"

namespace fe
{
	struct ACShader final : public AssetComponent
	{
		AssetID ShaderID;
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
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static AssetType GetTypeStatic() { return AssetType::MaterialAsset; }

		AssetID GetShaderID()               const { return Get<ACShader>().ShaderID; }
		void    SetShader(AssetID shaderID)       { Get<ACShader>().ShaderID = shaderID; }

		      std::vector<Uniform>& GetUniforms()       { return Get<ACUniforms>().Uniforms; }
		const std::vector<Uniform>& GetUniforms() const { return Get<ACUniforms>().Uniforms; }

		      std::vector<ShaderTextureSlot>& GetTextureSlots()       { return Get<ACShaderTextureSlots>().TextureSlots; }
		const std::vector<ShaderTextureSlot>& GetTextureSlots() const { return Get<ACShaderTextureSlots>().TextureSlots; }

		virtual void PlaceCoreComponents() final override;;
		virtual void Release() final override {};
		void SendDataToGPU(GDIType GDI, void* data) { };
		void UnloadFromCPU() {};

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