#pragma once

#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetInterface.h"
#include "FoolsEngine\Assets\AssetHandle.h"

#include "ShadingModel.h"

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"

namespace fe
{
	struct ACMaterialCore
	{
		AssetHandle<ShadingModel> ShadingModelHandle;
		std::vector<AssetHandle<Texture2D>> Textures;

		void* UniformsData = 0;
		size_t UniformsDataSize = 0;

		void Init();

		~ACMaterialCore() { if (UniformsData) operator delete(UniformsData); }
	};

	class MaterialObserver : public AssetInterface
	{
	public:
		const ACMaterialCore& GetCoreComponent() const { return Get<ACMaterialCore>(); }

		const void* GetUniformValuePtr(const ACMaterialCore& dataComponent, const Uniform& targetUniform) const { return GetUniformValuePtr_Internal(dataComponent, targetUniform); };
		const void* GetUniformValuePtr(const ACMaterialCore& dataComponent, const std::string& name) const { return GetUniformValuePtr_Internal(dataComponent, name); };

		AssetID GetTextureID(const ACMaterialCore& dataComponent, const ShaderTextureSlot& textureSlot) const;
		AssetID GetTextureID(const ACMaterialCore& dataComponent, const std::string& textureSlotName) const;

	protected:
		MaterialObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}

		void* GetUniformValuePtr_Internal(const ACMaterialCore& dataComponent, const Uniform& targetUniform) const;
		void* GetUniformValuePtr_Internal(const ACMaterialCore& dataComponent, const std::string& name) const;
	};
	
	class MaterialUser : public MaterialObserver
	{
	public:
		ACMaterialCore& GetCoreComponent() const { return Get<ACMaterialCore>(); }

		void MakeMaterial(const AssetObserver<ShadingModel>& shadingModelObserver) const;

		void* GetUniformValuePtr(const ACMaterialCore& dataComponent, const Uniform& targetUniform) const { return GetUniformValuePtr_Internal(dataComponent, targetUniform); };
		void* GetUniformValuePtr(const ACMaterialCore& dataComponent, const std::string& name) const { return GetUniformValuePtr_Internal(dataComponent, name); };

		void SetUniformValue(const ACMaterialCore& dataComponent, const Uniform& uniform, void* dataPointer) const;
		void SetUniformValue(const ACMaterialCore& dataComponent, const std::string& name, void* dataPointer) const;

		void SetTexture(ACMaterialCore& dataComponent, const ShaderTextureSlot& textureSlot, AssetID textureID) const;
		void SetTexture(ACMaterialCore& dataComponent, const std::string& textureSlotName, AssetID textureID) const;

		void ResetUniformValueToDefault(ACMaterialCore& dataComponent, const Uniform& uniform) const;

		bool SendDataToGPU(GDIType GDI) const;;
	protected:
		MaterialUser(ECS_AssetHandle ECS_handle) : MaterialObserver(ECS_handle) {}
	};

	class Material : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Material; }
		static constexpr const char* GetMetaFileExtension() { return ".femat"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::GetRegistry().emplace<ACMaterialCore>(assetID).Init(); }
		static void SaveMetadata(AssetID assetID);
		static bool LoadMetadata(AssetID assetID);

		using User = MaterialUser;
		using Observer = MaterialObserver;
		using Core = ACMaterialCore;
	};
}