#include "FE_pch.h"
#include "NativeScript.h"

#include "Scene.h"
#include "Hierarchy.h"

namespace fe
{
	HierarchicalComponentHandle<CTransform, Transform> NativeScript::GetTransformHandle()
	{
		return m_Scene->GetHierarchy().GetComponent<CTransform, Transform>(m_Handle.entity());
	}
	HierarchicalComponentHandle<CTags, Tags> NativeScript::GetTagsHandle()
	{
		return m_Scene->GetHierarchy().GetComponent<CTags, Tags>(m_Handle.entity());
	}

	// Destruction is scheduled

	void NativeScript::Destroy()
	{
		std::queue<SetID> toMark;
		toMark.push(GetSetID());

		SetID current;
		while (!toMark.empty())
		{
			current = toMark.front();
			toMark.pop();
			while (current != NullSetID)
			{
				auto& node = m_Scene->GetRegistry().get<CHierarchyNode>(current);
				if (!m_Scene->GetRegistry().all_of<CDestroyFlag>(current))
				{
					toMark.push(node.FirstChild);
					m_Scene->GetRegistry().emplace<CDestroyFlag>(current);
				}
				current = node.NextSibling;
			}
		}
	}
}