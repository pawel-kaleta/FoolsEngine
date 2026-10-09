#pragma once

#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"

#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"
#include "FoolsEngine/Renderer/1 - GAPI/Pipeline.h"


namespace YAML { class Emitter; }

namespace fe
{
	struct ACMaterial_Core;

	struct ACShadingModel_Core final : public AssetComponent
	{
		Render::GAPI::Raster::Specification mRasterSpec;
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

	template <Render::GAPI::Platform::ValueType tPlatform>
	struct ACShadingModel_GID final : public AssetComponent
	{
		Render::GAPI::GID mPipelineGID;
	};

	class ShadingModel : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::ShadingModel; }
		static constexpr const char* GetMetaFileExtension() { return ".fesm"; }

		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACShadingModel_Core>(assetID).Init(); }

		using Core = ACShadingModel_Core;
	};
}