#pragma once

#include "FoolsEngine\Scene\BaseEntity.h"

#include "Hierarchy\TagsHandle.h"
#include "Hierarchy\TransformHandle.h"
#include "Hierarchy\ChildrenList.h"

#include "GameplayWorld.h"

namespace fe
{
	class Actor;

	// GameplayWorld's entity
	class Entity : public BaseEntity
	{
	public:
		Entity(EntityID entityID, const GameplayWorld* world)
			: BaseEntity(entityID, (World*)world) {	}
		Entity(const Entity& other) = default;
		Entity(const BaseEntity& other);
		Entity() = default;

		void operator= (const BaseEntity& other);

		TransformHandle	GetTransformHandle() { return TransformHandle(ID(), m_Handle.registry()); }
		TagsHandle		GetTagsHandle() { return TagsHandle(ID(), m_Handle.registry()); }
		ChildrenList	GetChildrenList() { return ChildrenList(ID(), m_Handle.registry()); }

		// destruction is scheduled 
		void Destroy();

		bool IsHead();

		GameplayWorld* GetWorld() const { return (GameplayWorld*)m_World; }

		Entity	CreateChildEntity(const std::string& name = "Entity");
		Actor	CreateAttachedActor(const std::string& name = "Actor");

		EntityID GetHeadID()
		{
			auto* head = m_Handle.try_get<CHeadEntity>();
			if (head) { return head->HeadEntity; }
			return NullEntityID;
		}

		// In default storage pool only!
		// Be carefull about defferring, doing it twice in one frame with this method being second is undefined
		// use RemoveIfExist if you are unsure whether this component could be scheduled for destruction somewhere else before
		template<typename tnComponent>
		void Remove()
		{
			static_assert(!(std::is_base_of_v<CEntityNode, tnComponent>), "Cannot remove Hierarchical components!");

			FE_CORE_ASSERT(AllOf<tnComponent>(), "This Entity does not have this component");

			// if the whole entity is sceduled for destruction, no need to destroy component individually
			if (AllOf<CDestroyFlag>())
				return;

			GetWorld()->ScheduleComponentDestructionUnsafe<tnComponent>(&(Get<tnComponent>()), ID());
		}

		// In default storage pool only!
		// removal is scheduled for the end of the frame
		template<typename tnComponent>
		void RemoveIfExist()
		{
			static_assert(!(std::is_base_of_v<CEntityNode, tnComponent>), "Cannot remove Hierarchical components!");

			// if the whole entity is sceduled for destruction, no need to destroy component individually
			if (AllOf<CDestroyFlag>())
				return;

			tnComponent* component = GetIfExist<tnComponent>();

			if (component)
			{
				GetWorld()->ScheduleComponentDestruction<tnComponent>(component, ID());
			}
		}
	};
}