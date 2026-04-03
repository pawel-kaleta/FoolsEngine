#pragma once

#include "Texture.h"
#include "ShadingModel.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - Description/Buffer.h"
#include "FoolsEngine/Renderer/2 - Resource/StaticBuffer.h"

namespace YAML { class Emitter; class Node; }

namespace fe
{
	FE_DECLARE_ENUM(AlphaMode, Opaque, Cutout, Blend);

	struct ACMaterialCore
	{
		AssetID ShadingModelID;
		std::vector<AssetID> TextureIDs;

		void* UniformsData;
		size_t UniformsDataSize;

		void* UniformBufferData;
		size_t UniformBufferDataSize;

		void* ShaderStorageData;
		size_t ShaderStorageDataSize;

		void Init();

		~ACMaterialCore() { if (UniformsData) operator delete(UniformsData); }
	};

	class MaterialObserver : public AssetInterface
	{
	public:
		const ACMaterialCore& GetCore() const { return Get<ACMaterialCore>(); }

		const void* GetUniformValuePtr(const ACMaterialCore& dataComponent, const Description::Buffer::Element& targetUniform) const { return GetUniformValuePtr_Internal(dataComponent, targetUniform); };
		const void* GetUniformValuePtr(const ACMaterialCore& dataComponent, String name) const { return GetUniformValuePtr_Internal(dataComponent, name); };

		AssetID GetTextureID(const ACMaterialCore& dataComponent, const Description::ShaderInterface::TextureSampler& textureSampler) const;
		AssetID GetTextureID(const ACMaterialCore& dataComponent, String textureSamplerName) const;

		size_t GetCPUDataSize() const;
		size_t GetGPUDataSize() const { const auto& core = Get<ACMaterialCore>(); return core.UniformBufferDataSize + core.ShaderStorageDataSize; }
	protected:
		MaterialObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}

		void* GetUniformValuePtr_Internal(const ACMaterialCore& dataComponent, const Description::Buffer::Element& targetUniform) const;
		void* GetUniformValuePtr_Internal(const ACMaterialCore& dataComponent, String name) const;
	};
	
	class MaterialUser : public MaterialObserver
	{
	public:
		ACMaterialCore& GetCore() const { return Get<ACMaterialCore>(); }

		void MakeMaterial(const AssetObserver<ShadingModel>& shadingModelObserver) const;

		void* GetUniformValuePtr(const ACMaterialCore& dataComponent, const Description::Buffer::Element& targetUniform) const { return GetUniformValuePtr_Internal(dataComponent, targetUniform); };
		void* GetUniformValuePtr(const ACMaterialCore& dataComponent, String name) const { return GetUniformValuePtr_Internal(dataComponent, name); };

		void SetUniformValue(const ACMaterialCore& dataComponent, const Description::Buffer::Element& uniform, void* dataPointer) const;
		void SetUniformValue(const ACMaterialCore& dataComponent, String name, void* dataPointer) const;

		void SetTexture(ACMaterialCore& dataComponent, const Description::ShaderInterface::TextureSampler& textureSampler, AssetID textureID) const;
		void SetTexture(ACMaterialCore& dataComponent, String textureSamplerName, AssetID textureID) const;

		void ResetUniformValueToDefault(ACMaterialCore& dataComponent, const Description::Buffer::Element& uniform) const;

		bool SendDataToGPU(GAPIType GAPI) const;
		void Release() const;
		void UnloadFromCPU() const { };
	protected:
		MaterialUser(ECS_AssetHandle ECS_handle) : MaterialObserver(ECS_handle) {}
	};

	class Material : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Material; }
		static constexpr const char* GetMetaFileExtension() { return ".femat"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACMaterialCore>(assetID).Init(); }
		static void SaveMetadata(YAML::Emitter& emitter, AssetID assetID);
		static bool LoadMetadata(AssetID assetID);
		static AssetID LoadMetadataInternal(const YAML::Node& node, AssetID master, const std::filesystem::path& parentPath);

		static void MakeMaterial(AssetID assetID, const AssetObserver<ShadingModel>& shadingModelObserver);

		using User = MaterialUser;
		using Observer = MaterialObserver;
		using Core = ACMaterialCore;
	};
}