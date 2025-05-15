#pragma once

#include "Material.h"

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"

namespace fe
{
	struct ACMaterialInstanceData
	{
		AssetID MaterialID;
		std::vector<AssetID> Textures;

		void* UniformsData;
		size_t UniformsDataSize;

		void Init()
		{
			MaterialID = NullAssetID;
			Textures.clear();
			if (UniformsData) operator delete(UniformsData);
			UniformsData = nullptr;
			UniformsDataSize = 0;
		}

		~ACMaterialInstanceData() { if (UniformsData) operator delete(UniformsData); }
	};

	class MaterialInstance : public Asset
	{
	public:
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::MaterialInstanceAsset; }

		virtual void PlaceCoreComponent() final override { Emplace<ACMaterialData>().Init(); };
		virtual void Release() final override {};
		void SendDataToGPU(GDIType GDI, void* data) { };
		void UnloadFromCPU() {};

		void Init(const AssetObserver<Material>& materialObserver);

		const void* GetUniformValuePtr(const Uniform& targetUniform) const { return GetUniformValuePtr_Internal(targetUniform); };
		      void* GetUniformValuePtr(const Uniform& targetUniform)       { return GetUniformValuePtr_Internal(targetUniform); };

		const void* GetUniformValuePtr(const std::string& name) const { return GetUniformValuePtr_Internal(name); };
		      void* GetUniformValuePtr(const std::string& name)       { return GetUniformValuePtr_Internal(name); };;

		void SetUniformValue(const Uniform& uniform, void* dataPointer);
		void SetUniformValue(const std::string& name, void* dataPointer);

		AssetHandle<Texture2D> GetTexture(const ShaderTextureSlot& textureSlot) const;
		AssetHandle<Texture2D> GetTexture(const std::string& textureSlotName) const;
		void SetTexture(const ShaderTextureSlot& textureSlot, AssetID textureID);
		void SetTexture(const std::string& textureSlotName, AssetID textureID);

		const std::vector<AssetID>& GetTextures() const { return Get<ACMaterialInstanceData>().Textures; }
		      std::vector<AssetID>& GetTextures()       { return Get<ACMaterialInstanceData>().Textures; }
		
		AssetHandle<Material> GetMaterial() const { return AssetHandle<Material>(Get<ACMaterialInstanceData>().MaterialID); }


	protected:
		friend class AssetObserver<MaterialInstance>;
		friend class AssetUser<MaterialInstance>;
		MaterialInstance(ECS_AssetHandle ECS_handle) : Asset(ECS_handle) {}

	private:

		void* MaterialInstance::GetUniformValuePtr_Internal(const Uniform& targetUniform) const;
		void* MaterialInstance::GetUniformValuePtr_Internal(const std::string& name) const;
	};
}