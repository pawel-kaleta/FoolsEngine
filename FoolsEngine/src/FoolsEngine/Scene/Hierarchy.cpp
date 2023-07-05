#include "FE_pch.h"
#include "Hierarchy.h"

#include "Component.h"
#include "Set.h"
#include "Scene.h"

namespace fe
{
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

		FE_LOG_CORE_WARN("TO DO: Test sorting!");
		//Compare not really used, but entt api requires
		auto group = m_Registry.group<CHierarchyNode, CTransform, CTags, CName>();
		group.sort(Compare, Sort);
		m_SafeOrder = true;
	}
	void SceneHierarchy::DestroyFlagged()
	{
		FE_PROFILER_FUNC();

		auto view = m_Registry.view<CDestroyFlag>();

		for (auto setID : view)
		{
			m_Registry.destroy(setID);
		}

		m_SafeOrder = false;
	}
	void SceneHierarchy::CreateNode(Set set, SetID parentID, const std::string& name)
	{
		FE_PROFILER_FUNC();

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

		if (parentNode.FirstChild != NullSetID)
		{
			SetID nextChild = parentNode.FirstChild;
			SetID currentChild;
			CHierarchyNode* currentChildNode;

			do
			{
				currentChild = nextChild;
				currentChildNode = &m_Registry.get<CHierarchyNode>(currentChild);
				nextChild = currentChildNode->NextSibling;
			} while (nextChild != NullSetID && nextChild > set.ID());

			currentChildNode->NextSibling = set.ID();
			node.NextSibling = nextChild;
		}
		else
		{
			parentNode.FirstChild = set.ID();
		}

		m_SafeOrder = false;

	}
}
