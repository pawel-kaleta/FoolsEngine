#pragma once

#include "ComponentDestructionManager.h"
#include "System/SystemsDirector.h"
#include "Hierarchy/HierarchyDirector.h"

#include "FoolsEngine/Scene/World.h"
#include "FoolsEngine/Scene/SimulationStage.h"

namespace fe
{
	class GameplayWorld : public World
	{
	public:
		GameplayWorld();
		virtual void Initialize() final override;

		HierarchyDirector& GetHierarchy() const { return *m_Hierarchy.get(); }
		SystemsDirector& GetSystems()     const { return *m_SystemsDirector.get(); }

		Entity CreateEntity(EntityID parentEntity, String name = String("Entity",7));
		Actor CreateActor(EntityID attachmentEntityID, String name);
		Actor CreateActor(String name);
		Actor CreateActor(EntityID attachmentEntityID);
		Actor CreateActor();


		Actor CreateActorWithUUID(UUID uuid);

		auto AllEntitiesGroup() { return m_Hierarchy->Group(); }

		template <typename tnComponent>
		void ScheduleComponentDestruction(tnComponent* componentPointer, EntityID entityID)
		{
			m_DestructionManager.ScheduleRemoval<tnComponent>(entityID);
		}

		void DestroyScheduledComponents() { m_DestructionManager.DestroyComponents(m_Registry); }

		template <SimulationStage::ValueType stage>
		void Update();

		Entity	GetEntityWithPrimaryCamera() const;
		void	SetPrimaryCameraEntity(Entity entity);
		void	SetPrimaryCameraEntity(EntityID id);

		Entity	GetEntityWithPrimaryDirectionalLight() const;
		void	SetPrimaryDirectionalLightEntity(Entity entity);
		void	SetPrimaryDirectionalLightEntity(EntityID id);
	private:
		friend class Entity;
		friend class System;
		friend class SceneSerializerYAML;

		EntityID					m_PrimaryCameraEntityID;
		EntityID					m_PrimaryDirectionalLightEntityID;

		Scope<HierarchyDirector>	m_Hierarchy;
		Scope<SystemsDirector>      m_SystemsDirector;
		ComponentDestructionManager m_DestructionManager;

		template <typename tnComponent>
		void ScheduleComponentDestructionUnsafe(tnComponent* componentPointer, EntityID entityID)
		{
			m_DestructionManager.ScheduleErasure<tnComponent>(entityID);
		}

		template <SimulationStage::ValueType stage>
		bool IsActorUpdateEnrolled(EntityID actor)
		{
			const auto& storage = m_Registry.storage<CUpdateEnrollFlag<stage>>();
			return storage.contains(actor);
		}

		void UpdateActors(SimulationStage stage, bool (GameplayWorld::* updateEnrollCheck)(EntityID));
	};
}