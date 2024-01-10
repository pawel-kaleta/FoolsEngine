#pragma once
#include "ECS.h"
#include "Component.h"
#include "SimulationStages.h"
#include "World.h"

namespace fe
{
	class Entity;
	class UUID;

	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		Entity	GetEntityWithPrimaryCamera();
		void	SetPrimaryCameraEntity(Entity entity);
		void	SetPrimaryCameraEntity(EntityID id);

		void SimulationUpdate();
		void PostFrameUpdate();

		GameplayWorld* GetGameplayWorld() { return m_GameplayWorld.get(); }

	private:
		Scope<GameplayWorld>	m_GameplayWorld;
		EntityID				m_PrimaryCameraEntityID;
		
		template <typename tnSimulationStage>
		void Update();

		
		
	};


}