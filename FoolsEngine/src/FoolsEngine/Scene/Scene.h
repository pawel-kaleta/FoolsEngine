#pragma once
#include "ECS.h"

#include "FoolsEngine\Assets\Asset.h"

#include "FoolsEngine\Scene\GameplayWorld\GameplayWorld.h"
#include "FoolsEngine\Core\UUID.h"

namespace fe
{
	namespace SimulationStages
	{
		enum class Stage;
	}

	class Entity;

	struct ACWorlds : public AssetComponent
	{
		Scope<GameplayWorld> GameplayWorld;
	};

	// observer can be used for drawing editor panels and other stuff during editing/playing scene
	// user needed for initialization, serialization, deserialization and release
	class SceneObserver : public AssetInterface
	{
	public:
		virtual AssetType GetType() const override final { return GetTypeStatic(); }
		static  AssetType GetTypeStatic() { return AssetType::SceneAsset; }

		const ACWorlds& GetWorlds() const { return Get<ACWorlds>(); }

	protected:
		SceneObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class SceneUser : public SceneObserver
	{
	public:
		void PlaceCoreComponent() const;
		void Release() const;

		void Initialize() const;

		void SimulationUpdate() const;
		void PostFrameUpdate() const;

		ACWorlds& GetWorlds() const { return Get<ACWorlds>(); }

	private:
		template <SimulationStages::Stages stage>
		void Update() const;

	protected:
		SceneUser(ECS_AssetHandle ECS_handle) : SceneObserver(ECS_handle) {}
	};

	class Scene : public Asset
	{
	public:
		static  AssetType GetTypeStatic() { return AssetType::SceneAsset; }

		using Observer = SceneObserver;
		using User = SceneUser;
	};
}