#pragma once
#include "ECS.h"
#include "Hierarchy\EntitiesHierarchy.h"
#include "ComponentDestructionManager.h"
#include "SimulationStages.h"
#include "System.h"

#include <stack>

namespace fe
{
	class Scene;
	class Actor;
	class Entity;

	class World
	{
	public:
		World(Scene* scene, bool isGameplayWorld = false);

		Registry&	GetRegistry()		{ return m_Registry; }
		Scene*		GetScene()			{ return m_Scene; }
		bool		IsGameplayWorld()	{ return m_IsGameplayWorld; }

		EntityID TranslateID(UUID uuid);

		template <typename tnComponent>
		EntityID ComponentToEntity(const tnComponent& component)
		{
			return entt::to_entity(m_Registry, component);
		}
	protected:
		friend class Entity;
		friend class Actor;
		friend class EditorLayer;
		friend class SceneHierarchyPanel;

		Registry	m_Registry;
		Scene*		m_Scene				= nullptr;
		bool		m_IsGameplayWorld	= false;

		std::unordered_map<UUID, EntityID>	m_PersistentToTransientIDsMap;

		BaseEntity CreateEntity();
		BaseEntity CreateEntityWithUUID(UUID uuid);
	};

	class GameplayWorld : public World
	{
	public:
		GameplayWorld(Scene* scene);;

		EntitiesHierarchy& GetHierarchy() { return *m_Hierarchy.get(); }

		Entity CreateEntity(EntityID parentEntity, const std::string& name = "Entity");
		Actor CreateActor(EntityID attachmentEntityID = RootID, const std::string& name = "Actor");
		Actor CreateActor(const std::string& name);
		
		Actor CreateActorWithUUID(UUID uuid);

		auto AllEntitiesGroup() { return m_Hierarchy->Group(); }

		template <typename tnComponent>
		void ScheduleComponentDestruction(tnComponent* componentPointer, EntityID entityID)
		{
			m_DestructionManager.ScheduleRemoval<tnComponent>(entityID);
		}

		void DestroyScheduledComponents() {	m_DestructionManager.DestroyComponents(m_Registry); }

		template <typename tnSimulationStage>
		void Update()
		{
			UpdateActors(SimulationStages::EnumFromType<tnSimulationStage>(), &GameplayWorld::IsActorUpdateEnrolled<tnSimulationStage>);
			//UpdateSystems();
		}

	private:
		friend class Entity;

		Scope<EntitiesHierarchy>	m_Hierarchy;
		ComponentDestructionManager m_DestructionManager;
		
		template <typename tnComponent>
		void ScheduleComponentDestructionUnsafe(tnComponent* componentPointer, EntityID entityID)
		{
			m_DestructionManager.ScheduleDestructionUnsafe<tnComponent>(entityID);
		}

		void UpdateActors(SimulationStages::Stages stage, bool (GameplayWorld::* updateEnrollCheck)(EntityID));

		template <typename tnSimulationStage>
		bool IsActorUpdateEnrolled(EntityID actor)
		{
			const static auto& storage = m_Registry.storage<CUpdateEnrollFlag<tnSimulationStage>>();
			return storage.contains(actor);
		}

		using Systems = std::vector<std::unique_ptr<System>>;

		struct SystemUpdateEnroll
		{
			System* System;
			void (System::* OnUpdateFuncPtr)();
		};
		using SystemUpdateEnrolls = std::array<std::vector<SystemUpdateEnroll>, (int)SimulationStages::Stages::StagesCount>;

		Systems             m_Systems;
		SystemUpdateEnrolls	m_SystemUpdateEnrolls;

		template<typename tnSimulationStage>
		void EnrollForUpdate(System* system, void (System::* onUpdateFuncPtr)())
		{
			FE_LOG_CORE_DEBUG("GameplayWorld: system EnrollForUpdate");

			constexpr int stage = (int)SimulationStages::EnumFromType<tnSimulationStage>();
			m_SystemUpdateEnrolls[stage].push_back(SystemUpdateEnroll{ system, onUpdateFuncPtr });
		}
	};
}