#pragma once

#include "Texture.h"
#include "ShadingModel.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"


namespace YAML { class Emitter; class Node; }

namespace fe
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

		UInt DataSizeGPU() const
		{
			 return AlignOffsetTo<16>(mParamsData.Count);
		}
	};

	class Material : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Material; }
		static constexpr const char* GetMetaFileExtension() { return ".femat"; }

		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACMaterial_Core>(assetID).Init(); }

		using Core = ACMaterial_Core;
	};
}