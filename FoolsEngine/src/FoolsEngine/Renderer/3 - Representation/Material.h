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

		void* DefaultUniformsData;
		size_t UniformsDataSize;

		void Init();
	};

	class Material : public Asset
	{
	public:
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::MaterialAsset; }

		AssetID GetShaderID()               const { return Get<ACMaterialData>().ShaderID; }
		void    SetShader(AssetID shaderID)       { Get<ACMaterialData>().ShaderID = shaderID; }

	public:
		const void* GetUniformValuePtr(const Uniform& targetUniform) const { return GetUniformValuePtr_Internal(targetUniform); };
		      void* GetUniformValuePtr(const Uniform& targetUniform)       { return GetUniformValuePtr_Internal(targetUniform); };

		const void* GetUniformValuePtr(const std::string& name) const { return GetUniformValuePtr_Internal(name); };
		      void* GetUniformValuePtr(const std::string& name)       { return GetUniformValuePtr_Internal(name); };

		void SetUniformValue(const Uniform& uniform, void* dataPointer);
		void SetUniformValue(const std::string& name, void* dataPointer);

	private:
		void* GetUniformValuePtr_Internal(const Uniform& targetUniform) const;
		void* GetUniformValuePtr_Internal(const std::string& name) const;

	public:

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
	};

	

	
}