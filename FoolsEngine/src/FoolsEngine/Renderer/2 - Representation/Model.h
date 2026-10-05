#pragma once

#include "Mesh.h"
#include "RenderMesh.h"

#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Assets/AssetInterface.h"

namespace YAML { class Emitter; class Node; }

namespace fe::Render::Representation
{
	struct ACModel_Core final : public AssetComponent
	{
		Splice<AssetID> mRenderMeshIDs;

		void Init() { mRenderMeshIDs.Init(); }
	};

	class ModelObserver : public AssetInterface
	{
	public:
		const ACModel_Core& GetCore() const { return Get<ACModel_Core>(); }
		
	protected:
		ModelObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};
	
	class ModelUser : public ModelObserver
	{
	public:
		ACModel_Core& GetCore() const { return Get<ACModel_Core>(); }

	protected:
		ModelUser(ECS_AssetHandle ECS_handle) : ModelObserver(ECS_handle) {}
	};

	class Model : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Model; }
		static constexpr const char* GetMetaFileExtension() { return ".femodel"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::Get().m_Registry.emplace<ACModel_Core>(assetID).Init(); }

		using Observer = ModelObserver;
		using User = ModelUser;
		using Core = ACModel_Core;
	};
}