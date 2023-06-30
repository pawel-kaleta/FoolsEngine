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
	HierarchicalComponentHandle<CTags, TagsBase> NativeScript::GetTagsHandle()
	{
		return m_Scene->GetHierarchy().GetComponent<CTags, TagsBase>(m_Handle.entity());
	}
}