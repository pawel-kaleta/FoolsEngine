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



		void SimulationUpdate();
		void PostFrameUpdate();

		GameplayWorld* GetGameplayWorld() const { return m_GameplayWorld.get(); }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }
		const UUID GetUUID() const { return m_UUID; }
	private:
		friend class SceneSerializerYAML;

		UUID m_UUID;
		Scope<GameplayWorld> m_GameplayWorld;
		
		std::string          m_Name = "Untitled Scene";
		
		template <typename tnSimulationStage>
		void Update();
	};
}