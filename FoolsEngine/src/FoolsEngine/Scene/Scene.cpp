#include "FE_pch.h"
#include "Scene.h"
#include "Set.h"

namespace fe
{
	Scene::Scene()
		: m_PrimaryCameraSetID(0)
	{
		auto root = m_Registry.create(0);
		Set set(root, this);
		FE_CORE_ASSERT(set && (root == 0), "Failed to create root Set in a Scene");

		set.Emplace<CName>("root");
		set.Emplace<CCommonTags>(CommonTags::Root);
		set.Emplace<CTransform>();
	}

	Set Scene::CreateSet()
	{
		return Set(m_Registry.create(), this);
	}

	const Set Scene::Root()
	{
		return Set(0, this);
	}

	Set Scene::GetSetWithPrimaryCamera() {
		Set set(m_PrimaryCameraSetID, this);
		FE_CORE_ASSERT(set, "Set with primary m_Scene camera was deleted");
		FE_CORE_ASSERT(set.AllOf<CCamera>(), "Primary camera was removed from its Set");
		return set;
	}

	void Scene::SetPrimaryCameraSet(Set set)
	{
		if (!(set))
		{
			FE_CORE_ASSERT(false, "This is not a valid set");
			return;
		}

		if (!set.AllOf<CCamera>())
		{
			FE_CORE_ASSERT(false, "This set does not have a CCamera component");
			return;
		}

		if (set.m_Scene != this)
		{
			FE_CORE_ASSERT(false, "This set does not belong to this Scene");
			return;
		}

		m_PrimaryCameraSetID = set.ID();
	}

	void Scene::SetPrimaryCameraSet(SetID id)
	{
		Set set(id, this);
		SetPrimaryCameraSet(set);
	}
}