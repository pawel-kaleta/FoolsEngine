#include "FE_pch.h"
#include "Entity.h"

#include "GameplayWorld.h"
#include "Actor/Actor.h"


namespace fe
{
	Entity::Entity(const BaseEntity& other)
		: BaseEntity(other)
	{
		if (other)
		{
			FE_CORE_ASSERT(other.m_World->m_IsGameplayWorld, "This entity does not belong to GameplayWorld!");
		}
	}

	void Entity::operator=(const BaseEntity& other)
	{
		if (other)
		{
			FE_CORE_ASSERT(other.m_World->m_IsGameplayWorld, "This entity does not belong to GameplayWorld");
			m_Handle = other.m_Handle;
			m_World = other.m_World;
		}
		else
			*this = Entity();
	}

	Entity Entity::CreateChildEntity(const std::string& name)
	{
		return ((GameplayWorld*)m_World)->CreateEntity(ID(), name);
	}

	Actor Entity::CreateAttachedActor(const std::string& name)
	{
		FE_CORE_ASSERT(m_World->m_IsGameplayWorld, "Only Gamplay Worlds have actors!");

		return ((GameplayWorld*)m_World)->CreateActor(ID(), name);
	}

	bool Entity::IsHead() const { return AllOf<CActorData>(); }

	void Entity::Entity::Destroy()
	{
		FE_PROFILER_FUNC();

		std::queue<EntityID> entities_to_flag;
		auto& reg = m_World->m_Registry;
		reg.emplace<CDestroyFlag>(ID());
		entities_to_flag.push(m_Handle.get<CEntityNode>().FirstChild);

		EntityID current;
		while (!entities_to_flag.empty())
		{
			current = entities_to_flag.front();
			entities_to_flag.pop();
			while (current != NullEntityID)
			{
				auto& node = reg.get<CEntityNode>(current);
				if (!reg.all_of<CDestroyFlag>(current))
				{
					entities_to_flag.push(node.FirstChild);
					reg.emplace<CDestroyFlag>(current);
				}
				current = node.NextSibling;
			}
		}
	}
}