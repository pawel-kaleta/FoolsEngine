#include "FE_pch.h"
#include "Set.h"

#include "Scene.h"
#include "Hierarchy.h"

namespace fe
{
	Set::Set(SetID setID, Scene* scene)
		: m_Scene(scene), m_Handle(ECS_handle(scene->m_Registry, setID)) { }

	HierarchicalComponentHandle<CTransform, Transform> Set::GetTransformHandle()
	{
		return m_Scene->GetHierarchy().GetComponent<CTransform, Transform>(m_Handle.entity());
	}
	HierarchicalComponentHandle<CTags, Tags> Set::GetTagsHandle()
	{
		return m_Scene->GetHierarchy().GetComponent<CTags, Tags>(m_Handle.entity());
	}
	
	void Set::Destroy()
	{
		std::queue<SetID> toMark;
		toMark.push(ID());

		SetID current;
		while (!toMark.empty())
		{
			current = toMark.front();
			toMark.pop();
			while (current != NullSetID)
			{
				auto& node = m_Scene->m_Registry.get<CHierarchyNode>(current);
				if (!m_Scene->m_Registry.all_of<CDestroyFlag>(current))
				{
					toMark.push(node.FirstChild);
					m_Scene->m_Registry.emplace<CDestroyFlag>(current);
				}
				current = node.NextSibling;
			}
		}
	}
}