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
	class Scene : public Asset
	{
	public:
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static  AssetType GetTypeStatic() { return AssetType::SceneAsset; }

		virtual void PlaceCoreComponents() override final;
		virtual void Release() override final;
		void Initialize();

		void SimulationUpdate();
		void PostFrameUpdate();

		      ACWorlds& GetWorlds()       { return Get<ACWorlds>(); }
		const ACWorlds& GetWorlds() const { return Get<ACWorlds>(); }

	private:
		template <SimulationStages::Stages stage>
		void Update();

	protected:
		Scene(ECS_AssetHandle ECS_handle) : Asset(ECS_handle) {}
	};
}