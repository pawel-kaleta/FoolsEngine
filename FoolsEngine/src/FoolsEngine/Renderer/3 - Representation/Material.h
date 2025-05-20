#pragma once

#include "ShadingModel.h"

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"

namespace fe
{
	struct ACMaterialData
	{
		AssetID ShadingModelID;
		std::vector<AssetID> Textures;

		void* UniformsData;
		size_t UniformsDataSize;

		void Init()
		{
			ShadingModelID = NullAssetID;
			Textures.clear();
			if (UniformsData) operator delete(UniformsData);
			UniformsData = nullptr;
			UniformsDataSize = 0;
		}

		~ACMaterialData() { if (UniformsData) operator delete(UniformsData); }
	};

	class MaterialObserver : public AssetInterface
	{
	public:
		virtual AssetType GetType() const override final { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::MaterialAsset; }

		const void* GetUniformValuePtr(const Uniform& targetUniform) const { return GetUniformValuePtr_Internal(targetUniform); };
		const void* GetUniformValuePtr(const std::string& name) const { return GetUniformValuePtr_Internal(name); };

		AssetHandle<Texture2D> GetTexture(const ShaderTextureSlot& textureSlot) const;
		AssetHandle<Texture2D> GetTexture(const std::string& textureSlotName) const;

		const std::vector<AssetID>& GetTextures() const { return Get<ACMaterialData>().Textures; }

		AssetHandle<ShadingModel> GetShadingModel() const { return AssetHandle<ShadingModel>(Get<ACMaterialData>().ShadingModelID); }

	protected:
		MaterialObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}

		void* GetUniformValuePtr_Internal(const Uniform& targetUniform) const;
		void* GetUniformValuePtr_Internal(const std::string& name) const;
	};
	
	class MaterialUser : public MaterialObserver
	{
	public:
		void PlaceCoreComponent() const { Emplace<ACShadingModelData>().Init(); };

		void MakeMaterial(const AssetObserver<ShadingModel>& shadingModelObserver) const;

		void* GetUniformValuePtr(const Uniform& targetUniform) const { return GetUniformValuePtr_Internal(targetUniform); };
		void* GetUniformValuePtr(const std::string& name) const { return GetUniformValuePtr_Internal(name); };

		void SetUniformValue(const Uniform& uniform, void* dataPointer) const;
		void SetUniformValue(const std::string& name, void* dataPointer) const;

		void SetTexture(const ShaderTextureSlot& textureSlot, AssetID textureID) const;
		void SetTexture(const std::string& textureSlotName, AssetID textureID) const;

		std::vector<AssetID>& GetTextures() const { return Get<ACMaterialData>().Textures; }

	protected:
		MaterialUser(ECS_AssetHandle ECS_handle) : MaterialObserver(ECS_handle) {}
	};

	class Material : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::MaterialAsset; }

		using User = MaterialUser;
		using Observer = MaterialObserver;
	};
}