#pragma once

#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Renderer/1 - GAPI/Pipeline.h"


namespace YAML { class Emitter; }

namespace fe::Render::Representation
{
	struct ACMaterial_Core;

	struct ACShadingModel_Core final : public AssetComponent
	{
		GAPI::Raster::Specification mRasterSpec;
		Splice<Byte> mDefaultParamsData;
		Splice<Byte> mConstantsData;
		Splice<AssetID> mShaders;

		void (*mDeserializeMetadata)(ACShadingModel_Core* core, YAML::Node& data);
		void (*mSerializeMetadata)(ACShadingModel_Core* core, YAML::Node& data);
		void (*mDrawInspectorWidget)(ACShadingModel_Core* core);

		void (*mDeserializeMaterialMetadata)(ACMaterial_Core* core, YAML::Node& data);
		void (*mSerializeMaterialMetadata)(ACMaterial_Core* core, YAML::Node& data);
		void (*mDrawMaterialInspectorWidget)(ACMaterial_Core* core);

		void Init()
		{
			mRasterSpec.Init();
			mDefaultParamsData.Init();
			mConstantsData.Init();
			mShaders.Init();
			mDeserializeMetadata = nullptr;
			mSerializeMetadata = nullptr;
			mDrawInspectorWidget = nullptr;
			mDeserializeMaterialMetadata = nullptr;
			mSerializeMaterialMetadata = nullptr;
			mDrawMaterialInspectorWidget = nullptr;
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
	protected:
		ShadingModelObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class ShadingModelUser : public ShadingModelObserver
	{
	public:
		ACShadingModel_Core& GetCore() const { return Get<ACShadingModel_Core>(); }

	protected:
		ShadingModelUser(ECS_AssetHandle ECS_handle) : ShadingModelObserver(ECS_handle) {}
	};

	class ShadingModel : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::ShadingModel; }
		static constexpr const char* GetMetaFileExtension() { return ".fesm"; }

		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACShadingModel_Core>(assetID).Init(); }

		using User = ShadingModelUser;
		using Observer = ShadingModelObserver;
		using Core = ACShadingModel_Core;
	};
}