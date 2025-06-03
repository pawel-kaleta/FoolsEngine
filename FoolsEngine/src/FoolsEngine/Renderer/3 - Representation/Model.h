#pragma once

#include "FoolsEngine\Assets\AssetInterface.h"
#include "FoolsEngine\Assets\AssetHandle.h"

#include "Mesh.h"
#include "RenderMesh.h"

namespace fe
{
	struct ACModelCore final : public AssetComponent
	{
		std::vector<AssetID> RenderMeshes;

		void Init() { }
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
		static constexpr AssetType GetTypeStatic() { return AssetType::ModelAsset; }
		static void EmplaceCore(AssetID assetID) { AssetManager::GetRegistry().emplace<ACModelCore>(assetID).Init(); }
		static void Serialize(const AssetObserver<Model>& assetObserver);
		static bool Deserialize(AssetID assetID);

		using Observer = ModelObserver;
		using User = ModelUser;
		using Core = ACModelCore;
	};
}