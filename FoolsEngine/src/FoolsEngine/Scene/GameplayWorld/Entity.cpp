#include "FE_pch.h"

#include "Entity.h"

#include "FoolsEngine\Scene\GameplayWorld\Actor\Actor.h"
#include "GameplayWorld.h"


namespace fe
{
	Entity::Entity(const BaseEntity& other)
		: BaseEntity(other)
	{
		FE_CORE_ASSERT(other.m_World->IsGameplayWorld(), "This entity does not belong to GameplayWorld!");
	}

	void Entity::operator=(const BaseEntity& other)
	{
		FE_CORE_ASSERT(other.m_World->IsGameplayWorld(), "This entity does not belong to GameplayWorld");
		m_Handle = other.m_Handle;
		m_World = other.m_World;
	}

	Entity Entity::CreateChildEntity(const std::string& name)
	{
		return ((GameplayWorld*)m_World)->CreateEntity(ID(), name);
	}

	Actor Entity::CreateAttachedActor(const std::string& name)
	{
		FE_CORE_ASSERT(m_World->IsGameplayWorld(), "Only Gamplay Worlds have actors!");

		return ((GameplayWorld*)m_World)->CreateActor(ID(), name);
	}

	bool Entity::IsHead() { return AllOf<CActorData>(); }

	void Entity::Entity::Destroy()
	{
		FE_PROFILER_FUNC();

		std::queue<EntityID> toMark;
		m_World->GetRegistry().emplace<CDestroyFlag>(ID());
		toMark.push(m_Handle.get<CEntityNode>().FirstChild);

		EntityID current;
		while (!toMark.empty())
		{
			current = toMark.front();
			toMark.pop();
			while (current != NullEntityID)
			{
				auto& node = m_World->GetRegistry().get<CEntityNode>(current);
				if (!m_World->GetRegistry().all_of<CDestroyFlag>(current))
				{
					toMark.push(node.FirstChild);
					m_World->GetRegistry().emplace<CDestroyFlag>(current);
				}
				current = node.NextSibling;
			}
		}
	}
}