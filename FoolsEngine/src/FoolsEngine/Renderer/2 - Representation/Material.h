#pragma once

#include "Texture.h"
#include "ShadingModel.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"


namespace YAML { class Emitter; class Node; }

namespace fe::Render::Representation
{
	FE_DECLARE_ENUM(AlphaMode, Opaque, Cutout, Blend);

	struct ACMaterial_Core
	{
		AssetID mShadingModelID;
		Splice<AssetID> mTextureIDs;
		Splice<Byte> mParamsData;

		void Init()
		{
			mShadingModelID = NullAssetID;
			mTextureIDs.Init();
			mParamsData.Init();
		}
	};

	class MaterialObserver : public AssetInterface
	{
	public:
		const ACMaterial_Core& GetCore() const { return Get<ACMaterial_Core>(); }

	protected:
		MaterialObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};
	
	class MaterialUser : public MaterialObserver
	{
	public:
		ACMaterial_Core& GetCore() const { return Get<ACMaterial_Core>(); }

	protected:
		MaterialUser(ECS_AssetHandle ECS_handle) : MaterialObserver(ECS_handle) {}
	};

	class Material : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Material; }
		static constexpr const char* GetMetaFileExtension() { return ".femat"; }

		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACMaterial_Core>(assetID).Init(); }

		using User = MaterialUser;
		using Observer = MaterialObserver;
		using Core = ACMaterial_Core;
	};
}