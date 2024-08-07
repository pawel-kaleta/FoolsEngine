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
		void Initialize();

		void SimulationUpdate();
		void PostFrameUpdate();

		GameplayWorld* GetGameplayWorld() const { return m_GameplayWorld.get(); }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		const std::filesystem::path& GetFilepath() const { return m_Filepath; }
		void SetFilepath(const std::filesystem::path& path) { m_Filepath = path; }
		      
		UUID GetUUID() const { return m_UUID; }
	private:
		friend class SceneSerializerYAML;

		std::filesystem::path m_Filepath;
		std::string           m_Name = "Untitled Scene";
		UUID                  m_UUID;
		
		Scope<GameplayWorld>  m_GameplayWorld;
		
		template <SimulationStages::Stages stage>
		void Update();
	};
}