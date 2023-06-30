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
	HierarchicalComponentHandle<CTags, TagsBase> Set::GetTagsHandle()
	{
		return m_Scene->GetHierarchy().GetComponent<CTags, TagsBase>(m_Handle.entity());
	}
}