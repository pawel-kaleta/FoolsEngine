#pragma once

#include "FoolsEngine\Scene\World.h"

#include "Hierarchy\HierarchyDirector.h"
#include "ComponentDestructionManager.h"
#include "System\SystemsDirector.h"

namespace fe
{
	namespace SimulationStages
	{
		enum class Stages;
	}

	class GameplayWorld : public World
	{
	public:
		GameplayWorld(Scene* scene);
		virtual void Initialize() override;

		HierarchyDirector& GetHierarchy() { return *m_Hierarchy.get(); }
		SystemsDirector& GetSystems()     { return *m_SystemsDirector.get(); }

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

		void DestroyScheduledComponents() { m_DestructionManager.DestroyComponents(m_Registry); }

		template <SimulationStages::Stages stage>
		void Update();

		Entity	GetEntityWithPrimaryCamera() const;
		void	SetPrimaryCameraEntity(Entity entity);
		void	SetPrimaryCameraEntity(EntityID id);

	private:
		friend class Entity;
		friend class System;
		friend class SceneSerializerYAML;

		EntityID					m_PrimaryCameraEntityID;
		Scope<HierarchyDirector>	m_Hierarchy;
		Scope<SystemsDirector>      m_SystemsDirector;
		ComponentDestructionManager m_DestructionManager;

		template <typename tnComponent>
		void ScheduleComponentDestructionUnsafe(tnComponent* componentPointer, EntityID entityID)
		{
			m_DestructionManager.ScheduleErasure<tnComponent>(entityID);
		}

		template <SimulationStages::Stages stage>
		bool IsActorUpdateEnrolled(EntityID actor)
		{
			const auto& storage = m_Registry.storage<CUpdateEnrollFlag<stage>>();
			return storage.contains(actor);
		}

		void UpdateActors(SimulationStages::Stages stage, bool (GameplayWorld::* updateEnrollCheck)(EntityID));


	};
}