#include "FE_pch.h"
#include "HierarchyDirector.h"

#include "FoolsEngine/Scene/Scene.h"
#include "FoolsEngine/Scene/World.h"
#include "FoolsEngine/Scene/Component.h"
#include "FoolsEngine/Scene/GameplayWorld/Entity.h"
#include "FoolsEngine/Scene/GameplayWorld/Actor/ActorData.h"

namespace fe
{
	HierarchyDirector::HierarchyDirector(GameplayWorld* world)
		: m_World(world), m_Registry(&world->m_Registry)
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
		auto& parent_node = group.get<CEntityNode>(parentID);

		node.Parent = parentID;
		node.HierarchyLvl = parent_node.HierarchyLvl + 1;
		parent_node.ChildrenCount++;

		if (parent_node.FirstChild == NullEntityID)
		{
			parent_node.FirstChild = entity.ID();
			return;
		}

		EntityID current_child = parent_node.FirstChild;
		CEntityNode* current_child_node = & group.get<CEntityNode>(current_child);

		if (current_child > entity.ID())
		{
			parent_node.FirstChild = entity.ID();
			node.NextSibling = current_child;
			current_child_node->PreviousSibling = entity.ID();
			return;
		}

		EntityID nextChild = current_child_node->NextSibling;
		while (nextChild != NullEntityID && nextChild < entity.ID())
		{
			current_child = nextChild;
			current_child_node = &m_Registry->get<CEntityNode>(current_child);
			nextChild = current_child_node->NextSibling;
		} 

		if (nextChild != NullEntityID)
		{
			current_child_node->NextSibling = entity.ID();
			node.PreviousSibling = current_child;
			node.NextSibling = nextChild;
			m_Registry->get<CEntityNode>(nextChild).PreviousSibling = entity.ID();
		}
		else
		{
			current_child_node->NextSibling = entity.ID();
			node.PreviousSibling = current_child;
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
			auto [local, global, node] = group.get<CTransformLocal, CTransformGlobal, CEntityNode>(entityID);

			auto& parent_global = group.get<CTransformGlobal>(node.Parent);
			global.Transform = parent_global.Transform + local.Transform;
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

		auto& flag_storage = m_Registry->storage<CDestroyFlag>();
		
		if (flag_storage.size() == 0)
			return;
		
		flag_storage.sort(m_Compare);

		auto& node_storage = m_Registry->storage<CEntityNode>();

		for (auto entityID : flag_storage)
		{
			auto& node = node_storage.get(entityID);
			if (!flag_storage.contains(node.Parent) && m_Registry->valid(node.Parent))
			{
				if (node.PreviousSibling != NullEntityID)
				{
					auto& prev = node_storage.get(node.PreviousSibling);
					prev.NextSibling = node.NextSibling;
				}
				if (node.NextSibling != NullEntityID)
				{
					auto& next = node_storage.get(node.NextSibling);
					next.PreviousSibling = node.PreviousSibling;
				}

				auto& parent_node = node_storage.get(node.Parent);

				if (parent_node.FirstChild == entityID)
					parent_node.FirstChild = node.NextSibling;

				--parent_node.ChildrenCount;
			}
			m_Registry->destroy(entityID);
		}
	}
}
