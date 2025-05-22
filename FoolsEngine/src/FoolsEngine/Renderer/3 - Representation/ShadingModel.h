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

		const ACShadingModelData& GetDataComponent() const { return Get<ACShadingModelData>(); }

		const void* GetUniformValuePtr(const ACShadingModelData& dataComponent, const Uniform& targetUniform) const { return GetUniformValuePtr_Internal(dataComponent, targetUniform); };
		const void* GetUniformValuePtr(const ACShadingModelData& dataComponent, const std::string& name) const { return GetUniformValuePtr_Internal(dataComponent, name); };

	protected:
		ShadingModelObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}

		void* GetUniformValuePtr_Internal(const ACShadingModelData& dataComponent, const Uniform& targetUniform) const;
		void* GetUniformValuePtr_Internal(const ACShadingModelData& dataComponent, const std::string& name) const;
	};

	class ShadingModelUser : public ShadingModelObserver
	{
	public:
		void PlaceCoreComponent() const { Emplace<ACShadingModelData>().Init(); }
		
		void MakeShadingModel(
			AssetID shaderID,
			const std::initializer_list<Uniform>& uniforms,
			const std::initializer_list<ShaderTextureSlot>& textureSlots) const; // this needs enforcing of setting default valus for uniforms
		
		ACShadingModelData& GetDataComponent() const { return Get<ACShadingModelData>(); }

		void* GetUniformValuePtr(const ACShadingModelData& dataComponent, const Uniform& targetUniform) const { return GetUniformValuePtr_Internal(dataComponent, targetUniform); };
		void* GetUniformValuePtr(const ACShadingModelData& dataComponent, const std::string& name) const { return GetUniformValuePtr_Internal(dataComponent, name); };

		void SetUniformValue(const ACShadingModelData& dataComponent, const Uniform& uniform, void* dataPointer) const;
		void SetUniformValue(const ACShadingModelData& dataComponent, const std::string& name, void* dataPointer) const;

	protected:
		ShadingModelUser(ECS_AssetHandle ECS_handle) : ShadingModelObserver(ECS_handle) {}

	private:
		void InitUniformValue(const Uniform& uniform, void* dataPointer) const;
	};

	class ShadingModel : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::ShadingModelAsset; }

		using User = ShadingModelUser;
		using Observer = ShadingModelObserver;
	};


}