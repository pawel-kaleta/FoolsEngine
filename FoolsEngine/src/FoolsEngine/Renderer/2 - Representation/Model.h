#pragma once

#include "Mesh.h"
#include "RenderMesh.h"

#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"

namespace YAML { class Emitter; class Node; }

namespace fe
{
	struct ACModel_Core final : public AssetComponent
	{
		Splice<AssetID> mRenderMeshIDs;

		void Init() { mRenderMeshIDs.Init(); }
	};

	class Model : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Model; }
		static constexpr const char* GetMetaFileExtension() { return ".femodel"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACModel_Core>(assetID).Init(); }

		using Core = ACModel_Core;
	};
}