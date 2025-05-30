#pragma once
#include "ECS.h"

#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetInterface.h"

#include "FoolsEngine\Scene\GameplayWorld\GameplayWorld.h"

namespace fe
{
	namespace SimulationStages
	{
		enum class Stage;
	}

	class Entity;

	struct ACSceneCore : public AssetComponent
	{
		Scope<GameplayWorld> GameplayWorld;

		void Init() { }
	};

	// observer can be used for drawing editor panels and other stuff during editing/playing scene
	// user needed for initialization, serialization, deserialization and release
	class SceneObserver : public AssetInterface
	{
	public:
		const ACSceneCore& GetCoreComponent() const { return Get<ACSceneCore>(); }

	protected:
		SceneObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class SceneUser : public SceneObserver
	{
	public:
		ACSceneCore& GetCoreComponent() const { return Get<ACSceneCore>(); }
		
		void Release() const;

		void Initialize() const;

		void SimulationUpdate() const;
		void PostFrameUpdate() const;

	private:
		template <SimulationStages::Stages stage>
		void Update() const;

	protected:
		SceneUser(ECS_AssetHandle ECS_handle) : SceneObserver(ECS_handle) {}
	};

	class Scene : public Asset
	{
	public:
		static AssetType GetTypeStatic() { return AssetType::SceneAsset; }
		static void EmplaceCore(AssetID assetID) { AssetManager::GetRegistry().emplace<ACSceneCore>(assetID).Init(); }

		using Observer = SceneObserver;
		using User = SceneUser;
		using Core = ACSceneCore;
	};
}