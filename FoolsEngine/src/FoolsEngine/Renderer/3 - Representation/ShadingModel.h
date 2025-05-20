#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Shader.h"

#include "FoolsEngine\Assets\AssetHandle.h"

namespace fe
{
	struct ACShadingModelData final : public AssetComponent
	{
		AssetID ShaderID;
		std::vector<Uniform> Uniforms;
		std::vector<ShaderTextureSlot> TextureSlots;

		void* DefaultUniformsData;
		size_t UniformsDataSize;

		void Init();

		~ACShadingModelData() { if (DefaultUniformsData) operator delete(DefaultUniformsData); }
	};

	class ShadingModelObserver : public AssetInterface
	{
	public:
		virtual AssetType GetType() const override final { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::ShadingModelAsset; }

		AssetID GetShaderID() const { return Get<ACShadingModelData>().ShaderID; }

		const void* GetUniformValuePtr(const Uniform& targetUniform) const { return GetUniformValuePtr_Internal(targetUniform); };
		const void* GetUniformValuePtr(const std::string& name) const { return GetUniformValuePtr_Internal(name); };

		const std::vector<Uniform>& GetUniforms() const { return Get<ACShadingModelData>().Uniforms; }
		const std::vector<ShaderTextureSlot>& GetTextureSlots() const { return Get<ACShadingModelData>().TextureSlots; }
	
	protected:
		ShadingModelObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}

		void* GetUniformValuePtr_Internal(const Uniform& targetUniform) const;
		void* GetUniformValuePtr_Internal(const std::string& name) const;
	};

	class ShadingModelUser : public ShadingModelObserver
	{
	public:
		void PlaceCoreComponent() const { Emplace<ACShadingModelData>().Init(); }
		
		void MakeShadingModel(
			AssetID shaderID,
			const std::initializer_list<Uniform>& uniforms,
			const std::initializer_list<ShaderTextureSlot>& textureSlots) const; // this needs setting default valus for uniforms
		
		void SetShader(AssetID shaderID) const { Get<ACShadingModelData>().ShaderID = shaderID; }

		void* GetUniformValuePtr(const Uniform& targetUniform) const { return GetUniformValuePtr_Internal(targetUniform); };
		void* GetUniformValuePtr(const std::string& name) const { return GetUniformValuePtr_Internal(name); };

		void SetUniformValue(const Uniform& uniform, void* dataPointer) const;
		void SetUniformValue(const std::string& name, void* dataPointer) const;

		std::vector<Uniform>& GetUniforms() const { return Get<ACShadingModelData>().Uniforms; }
		std::vector<ShaderTextureSlot>& GetTextureSlots() const { return Get<ACShadingModelData>().TextureSlots; }

	protected:
		ShadingModelUser(ECS_AssetHandle ECS_handle) : ShadingModelObserver(ECS_handle) {}
	};

	class ShadingModel : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::ShadingModelAsset; }

		using User = ShadingModelUser;
		using Observer = ShadingModelObserver;
	};


}