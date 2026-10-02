#pragma once

#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Renderer/1 - GAPI/Pipeline.h"


namespace YAML { class Emitter; }

namespace fe::Render::Representation
{
	struct ACShadingModel_Core final : public AssetComponent
	{
		GAPI::Raster::Specification mRasterSpec;
		Splice<Byte> DefaultParamsData;
		Splice<AssetID> mShaders;

		void Init()
		{
			mRasterSpec.Init();
			DefaultParamsData = Splice<Byte>();
		}
	};

	template <GAPI::Platform::ValueType tPlatform>
	struct ACShadingModel_GID final : public AssetComponent
	{
		GAPI::GID mPipelineGID;
	};

	class ShadingModelObserver : public AssetInterface
	{
	public:
		const ACShadingModel_Core& GetCore() const { return Get<ACShadingModel_Core>(); }

		void SaveMetadata(YAML::Emitter& emitter);

	protected:
		ShadingModelObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class ShadingModelUser : public ShadingModelObserver
	{
	public:
		ACShadingModel_Core& GetCore() const { return Get<ACShadingModel_Core>(); }

		bool LoadBaseAssetMetadata(const char* filepath);
		bool LoadMetadata();

		void UnloadFromCPU() const {};
		void Release() const;

	protected:
		ShadingModelUser(ECS_AssetHandle ECS_handle) : ShadingModelObserver(ECS_handle) {}
	};

	class ShadingModel : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::ShadingModel; }
		static constexpr const char* GetMetaFileExtension() { return ".fesm"; }
		static void SaveMetadata(YAML::Emitter& emitter, AssetID assetID) {}
		static bool LoadMetadata(AssetID assetID) { return false; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACShadingModel_Core>(assetID).Init(); }

		using User = ShadingModelUser;
		using Observer = ShadingModelObserver;
		using Core = ACShadingModel_Core;
	};
}