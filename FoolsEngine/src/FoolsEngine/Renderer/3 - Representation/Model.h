#pragma once

#include "FoolsEngine\Assets\AssetInterface.h"
#include "FoolsEngine\Assets\AssetHandle.h"

#include "Mesh.h"
#include "RenderMesh.h"

namespace fe
{
	struct ACModelCore final : public AssetComponent
	{
		std::vector<AssetID> RenderMeshIDs;

		void Init()	{ }
	};

	class ModelObserver : public AssetInterface
	{
	public:
		const ACModelCore& GetCoreComponent() const { return Get<ACModelCore>(); }
		
	protected:
		ModelObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};
	
	class ModelUser : public ModelObserver
	{
	public:
		void PlaceCoreComponent() const
		{
			Emplace<ACModelCore>();
		}

		ACModelCore& GetCoreComponent() const { return Get<ACModelCore>(); }

	protected:
		ModelUser(ECS_AssetHandle ECS_handle) : ModelObserver(ECS_handle) {}
	};

	class Model : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Model; }
		static constexpr const char* GetMetaFileExtension() { return ".femodel"; }
		static void EmplaceCore(AssetID assetID) { AssetManager::GetRegistry().emplace<ACModelCore>(assetID).Init(); }
		static void SaveMetadata(AssetID assetID);
		static bool LoadMetadata(AssetID assetID);

		using Observer = ModelObserver;
		using User = ModelUser;
		using Core = ACModelCore;
	};
}