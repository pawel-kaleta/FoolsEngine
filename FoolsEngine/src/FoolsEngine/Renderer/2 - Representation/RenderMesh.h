#pragma once

#include "Mesh.h"

#include "FoolsEngine/Assets/Asset.h"
#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"
#include "FoolsEngine/Assets/AssetAccessors.h"

namespace YAML { class Emitter; class Node; }

namespace fe
{
	struct ACRenderMeshCore final : public AssetComponent
	{
		AssetID mMeshID;
		AssetID mMaterialID;

		void Init() { mMeshID = NullAssetID; mMaterialID = NullAssetID; }
	};

	class RenderMesh final : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::RenderMesh; }
		static constexpr const char* GetMetaFileExtension() { return ".ferm"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACRenderMeshCore>(assetID).Init(); }

		using Core = ACRenderMeshCore;
	};
}