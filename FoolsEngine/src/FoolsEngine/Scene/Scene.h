#pragma once
#include "ECS.h"

#include "SimulationStages.h"
#include "World.h"
#include "FoolsEngine\Scene\GameplayWorld\GameplayWorld.h"

namespace fe
{
	class Entity;
	class UUID;

	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		Entity	GetEntityWithPrimaryCamera() const;
		void	SetPrimaryCameraEntity(Entity entity);
		void	SetPrimaryCameraEntity(EntityID id);

		void SimulationUpdate();
		void PostFrameUpdate();

		GameplayWorld* GetGameplayWorld() const { return m_GameplayWorld.get(); }

		const std::string& GetName() const { return m_Name; }
	private:
		Scope<GameplayWorld> m_GameplayWorld;
		EntityID             m_PrimaryCameraEntityID;
		std::string          m_Name = "Untitled Scene";
		
		template <typename tnSimulationStage>
		void Update();
	};
}