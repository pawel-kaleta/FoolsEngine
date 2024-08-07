#include "FE_pch.h"
#include "HierarchyDirector.h"

#include "FoolsEngine\Scene\Component.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "FoolsEngine\Scene\GameplayWorld\Actor\ActorData.h"
#include "FoolsEngine\Scene\Scene.h"
#include "FoolsEngine\Scene\World.h"

namespace fe
{
	HierarchyDirector::HierarchyDirector(GameplayWorld* world)
		: m_World(world), m_Registry(&world->GetRegistry())
	{
		FE_PROFILER_FUNC();

		auto group = Group();
		FE_CORE_ASSERT(group, "");
	}

	void HierarchyDirector::CreateNode(EntityID entityID, EntityID parentID)
	{
		FE_PROFILER_FUNC();

		Entity entity(entityID, m_World);

		m_SafeOrder = false;
		auto group = Group();

		entity.m_Handle.emplace<CTags>().Global = Entity(parentID, m_World).Get<CTags>().Global;

		entity.m_Handle.emplace<CTransformLocal>();
		entity.m_Handle.emplace<CTransformGlobal>();
		entity.m_Handle.emplace<CDirtyFlag<CTransformGlobal>>();
		//entity.m_Handle.emplace<CHeadEntity>().HeadEntity = group.get<CHeadEntity>(parentID).HeadEntity;

		auto& node = entity.m_Handle.emplace<CEntityNode>();
		auto& parentNode = group.get<CEntityNode>(parentID);

		node.Parent = parentID;
		node.HierarchyLvl = parentNode.HierarchyLvl + 1;
		parentNode.ChildrenCount++;

		if (parentNode.FirstChild == NullEntityID)
		{
			parentNode.FirstChild = entity.ID();
			return;
		}

		EntityID currentChild = parentNode.FirstChild;
		CEntityNode* currentChildNode = & group.get<CEntityNode>(currentChild);

		if (currentChild > entity.ID())
		{
			parentNode.FirstChild = entity.ID();
			node.NextSibling = currentChild;
			currentChildNode->PreviousSibling = entity.ID();
			return;
		}

		EntityID nextChild = currentChildNode->NextSibling;
		while (nextChild != NullEntityID && nextChild < entity.ID())
		{
			currentChild = nextChild;
			currentChildNode = &m_Registry->get<CEntityNode>(currentChild);
			nextChild = currentChildNode->NextSibling;
		} 

		if (nextChild != NullEntityID)
		{
			currentChildNode->NextSibling = entity.ID();
			node.PreviousSibling = currentChild;
			node.NextSibling = nextChild;
			m_Registry->get<CEntityNode>(nextChild).PreviousSibling = entity.ID();
		}
		else
		{
			currentChildNode->NextSibling = entity.ID();
			node.PreviousSibling = currentChild;
			node.NextSibling = nextChild;
		}
	}

	void HierarchyDirector::MakeGlobalTransformsCurrent()
	{
		FE_PROFILER_FUNC();

		{
			FE_PROFILER_SCOPE("std::sort");
			m_Registry->sort<CDirtyFlag<CTransformGlobal>>(m_Compare);
		}

		auto view = m_Registry->view<CDirtyFlag<CTransformGlobal>>();
		auto group = Group();

		for (auto entityID : view)
		{
			auto& [local, global, node] = group.get<CTransformLocal, CTransformGlobal, CEntityNode>(entityID);

			auto& parentGlobal = group.get<CTransformGlobal>(node.Parent);
			//auto& parentGlobal = m_Registry->get<CTransformGlobal>(node.Parent);
			global.Transform = parentGlobal.Transform + local.Transform;
		}

		m_Registry->storage<CDirtyFlag<CTransformGlobal>>().clear();
	}

	void HierarchyDirector::RecreateStorageOrder()
	{
		FE_PROFILER_FUNC();

		//m_Compare not really used, but entt api requires
		auto group = Group();
		group.sort(m_Compare, m_Sort);
		m_SafeOrder = true;
	}

	void HierarchyDirector::DestroyFlagged()
	{
		FE_PROFILER_FUNC();

		auto& flagStorage = m_Registry->storage<CDestroyFlag>();
		
		if (flagStorage.size() == 0)
			return;
		
		flagStorage.sort(m_Compare);

		auto& nodeStorage = m_Registry->storage<CEntityNode>();

		for (auto entityID : flagStorage)
		{
			auto& node = nodeStorage.get(entityID);
			if (!flagStorage.contains(node.Parent) && m_Registry->valid(node.Parent))
			{
				if (node.PreviousSibling != NullEntityID)
				{
					auto& prev = nodeStorage.get(node.PreviousSibling);
					prev.NextSibling = node.NextSibling;
				}
				if (node.NextSibling != NullEntityID)
				{
					auto& next = nodeStorage.get(node.NextSibling);
					next.PreviousSibling = node.PreviousSibling;
				}

				auto& parentNode = nodeStorage.get(node.Parent);

				if (parentNode.FirstChild == entityID)
					parentNode.FirstChild = node.NextSibling;

				--parentNode.ChildrenCount;
			}
			m_Registry->destroy(entityID);
		}
	}
}
