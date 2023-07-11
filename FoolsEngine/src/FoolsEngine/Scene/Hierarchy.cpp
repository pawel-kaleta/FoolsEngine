#include "FE_pch.h"
#include "Hierarchy.h"

#include "Component.h"
#include "Set.h"
#include "Scene.h"

namespace fe
{
	uint32_t SceneHierarchy::sort::m_AStep = 0;

	void SceneHierarchy::MakeGlobalTagsCurrent()
	{
		MakeGlobalsCurrent<CTags>();
	}
	void SceneHierarchy::MakeGlobalTransformsCurrent()
	{
		MakeGlobalsCurrent<CTransform>();
	}
	 void SceneHierarchy::MakeHierarchyCurrent()
	{
		MakeGlobalsCurrent<CTags>();
		MakeGlobalsCurrent<CTransform>();
	}
	void SceneHierarchy::SortStep()
	{
		FE_PROFILER_FUNC();

		//Compare not really used, but entt api requires
		auto group = m_Registry.group<CHierarchyNode, CTransform, CTags, CName>();
		group.sort(Compare, Sort);
		m_SafeOrder = true;
	}
	void SceneHierarchy::DestroyFlagged()
	{
		FE_PROFILER_FUNC();

		auto& storage = m_Registry.storage<CDestroyFlag>();
		
		if (storage.size() == 0)
			return;

		storage.sort(Compare);

		auto view = m_Registry.view<CDestroyFlag, CHierarchyNode>();

		for (auto it = storage.begin(); it != storage.end(); ++it)
		{
			auto& node = view.get<CHierarchyNode>(*it);

			if (!storage.contains(node.Parent))
			{
				if (node.PreviousSibling != NullSetID)
				{
					auto& prev = view.get<CHierarchyNode>(node.PreviousSibling);
					prev.NextSibling = node.NextSibling;
				}
				if (node.NextSibling != NullSetID)
				{
					auto& next = view.get<CHierarchyNode>(node.NextSibling);
					next.PreviousSibling = node.PreviousSibling;
				}
				
				auto& parentNode = view.get<CHierarchyNode>(node.Parent);
				
				if (parentNode.FirstChild == *it)
					parentNode.FirstChild = node.NextSibling;
				if (parentNode.LastChild == *it)
					parentNode.LastChild = node.PreviousSibling;
				if (parentNode.FirstChild == NullSetID)
					parentNode.FirstChild = parentNode.LastChild;
				if (parentNode.LastChild == NullSetID)
					parentNode.LastChild = parentNode.FirstChild;

				--parentNode.Children;
			}

			m_Registry.destroy(*it);
		}
	}

	void SceneHierarchy::CreateNode(Set set, SetID parentID, const std::string& name)
	{
		FE_PROFILER_FUNC();

		m_SafeOrder = false;

		set.m_Handle.emplace<CTags>();
		set.m_Handle.emplace<CDirtyFlag<CTags>>();

		set.m_Handle.emplace<CTransform>();
		set.m_Handle.emplace<CDirtyFlag<CTransform>>();

		set.m_Handle.emplace<CName>(name);

		auto& node = set.m_Handle.emplace<CHierarchyNode>();
		auto& parentNode = m_Registry.get<CHierarchyNode>(parentID);

		node.Parent = parentID;
		node.HierarchyLvl = parentNode.HierarchyLvl + 1;
		parentNode.Children++;

		if (parentNode.FirstChild == NullSetID)
		{
			parentNode.FirstChild = set.ID();
			return;
		}

		SetID currentChild = parentNode.FirstChild;
		CHierarchyNode* currentChildNode = &m_Registry.get<CHierarchyNode>(currentChild);

		if (currentChild > set.ID())
		{
			parentNode.FirstChild = set.ID();
			node.NextSibling = currentChild;
			currentChildNode->PreviousSibling = set.ID();
			return;
		}

		SetID nextChild = currentChildNode->NextSibling;
		while (nextChild != NullSetID && nextChild < set.ID())
		{
			currentChild = nextChild;
			currentChildNode = &m_Registry.get<CHierarchyNode>(currentChild);
			nextChild = currentChildNode->NextSibling;
		} 

		if (nextChild != NullSetID)
		{
			currentChildNode->NextSibling = set.ID();
			node.PreviousSibling = currentChild;
			node.NextSibling = nextChild;
			m_Registry.get<CHierarchyNode>(nextChild).PreviousSibling = set.ID();
		}
		else
		{
			currentChildNode->NextSibling = set.ID();
			node.PreviousSibling = currentChild;
			node.NextSibling = nextChild;
		}
	}
}
