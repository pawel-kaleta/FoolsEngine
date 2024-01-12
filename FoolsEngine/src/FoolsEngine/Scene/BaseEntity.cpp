#include "FE_pch.h"
#include "BaseEntity.h"

#include "World.h"

namespace fe
{
	BaseEntity::BaseEntity(EntityID entityID, World* world)
		: m_World(world), m_Handle(ECS_handle(world->GetRegistry(), entityID)) { }
}