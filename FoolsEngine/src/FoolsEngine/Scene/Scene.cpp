#include "FE_pch.h"
#include "ECS.h"
#include "Scene.h"

#include "Set.h"
#include "Hierarchy.h"
#include "NativeScript.h"

namespace fe
{
	Scene::Scene()
		: m_PrimaryCameraSetID(NullSetID)
	{
		auto root = m_Registry.create(RootID);
		ECS_handle handle(m_Registry, RootID);
		
		FE_CORE_ASSERT(handle.valid() && handle.entity() == RootID, "Failed to create root Set in a Scene");

		handle.emplace<CName>("root");
		handle.emplace<CTransform>();
		handle.emplace<CTags>();
		handle.emplace<CHierarchyNode>().Parent = NullSetID;

		m_Hierarchy = CreateScope<SceneHierarchy>(m_Registry);
	}

	Set Scene::CreateSet(SetID parent, const std::string& name)
	{
		Set set(m_Registry.create(), this);
		
		m_Hierarchy->CreateNode(set, parent, name);

		return set;
	}

	const Set Scene::Root()
	{
		return Set(RootID, this);
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

	void Scene::UpdateScripts()
	{
		FE_PROFILER_FUNC();

		auto& scriptComponentsStorage = m_Registry.storage<CNativeScript>();
		auto& querry = ComponentsQuerry(std::forward_as_tuple(scriptComponentsStorage), std::forward_as_tuple());
		for (auto setID : querry)
		{
			CNativeScript& script = querry.get<CNativeScript>(setID);
			script.m_Instance->OnUpdate();
		}
	}
	void Scene::DestroyFlaggedSets()
	{
		m_Hierarchy->DestroyFlagged();
	}
}