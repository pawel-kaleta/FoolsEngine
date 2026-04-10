#pragma once

#include "Texture.h"
#include "ShadingModel.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - Description/Buffer.h"
#include "FoolsEngine/Renderer/2 - Resource/RStaticBuffer.h"

namespace YAML { class Emitter; class Node; }

namespace fe
{
	FE_DECLARE_ENUM(AlphaMode, Opaque, Cutout, Blend);

	struct ACMaterialCore
	{
		AssetID ShadingModelID;
		Splice<AssetID> TextureIDs;

		Splice<Byte> UniformsData;
		Splice<Byte> UniformBufferData;
		Splice<Byte> ShaderStorageData;

		void Init() { ShadingModelID = NullAssetID; }
	};

	class MaterialObserver : public AssetInterface
	{
	public:
		const ACMaterialCore& GetCore() const { return Get<ACMaterialCore>(); }

		Splice<Byte> GetUniformValue(const Description::Buffer::Element& targetUniform) const;
		Splice<Byte> GetUniformValue(String name) const;

		AssetID GetTextureID(const Description::ShaderInterface::TextureSampler& textureSampler) const;
		AssetID GetTextureID(String textureSamplerName) const;

		UInt GetCPUDataSize() const { return GetCore().UniformsData.Count; }
		UInt GetGPUDataSize() const { const auto& core = Get<ACMaterialCore>(); return core.UniformBufferData.Count + core.ShaderStorageData.Count; }
	protected:
		MaterialObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};
	
	class MaterialUser : public MaterialObserver
	{
	public:
		ACMaterialCore& GetCore() const { return Get<ACMaterialCore>(); }

		void MakeMaterial(const AssetObserver<ShadingModel>& shadingModelObserver) const;

		void SetUniformValue(const Description::Buffer::Element& uniform, Splice<Byte> data) const;
		void SetUniformValue(String name, Splice<Byte> data) const;

		void SetTexture(const Description::ShaderInterface::TextureSampler& textureSampler, AssetID textureID) const;
		void SetTexture(String textureSamplerName, AssetID textureID) const;

		void ResetUniformValueToDefault(const Description::Buffer::Element& uniform) const;

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