#include "FE_pch.h"

#include "TransformHandle.h"

namespace fe
{
	TransformHandle::TransformHandle(EntityID ID, Registry* registry)
		:
		m_Local(registry->get<CTransformLocal>(ID)),
		m_Global(registry->get<CTransformGlobal>(ID)),
		m_Node(registry->get<CEntityNode>(ID)),
		m_EntityID(ID),
		m_Registry(registry)
	{
		FE_CORE_ASSERT(m_Node.Parent != NullEntityID, "Node does not have a parent!");

		if (m_Node.Parent == RootID)
			m_ParentRoot = true;
	}

	void TransformHandle::SetLocal(const Transform& other)
	{
		FE_PROFILER_FUNC();

		m_Local.Transform = other;
		if (m_ParentRoot)
		{
			m_Global.Transform = other;
		}
		else if (!IsDirty(m_EntityID))
		{
			SetDirty(m_EntityID);
		}
		MarkDescendantsDirty();
	}

	void TransformHandle::SetGlobal(const Transform& other)
	{
		FE_PROFILER_FUNC();

		if (!IsDirty())
		{
			if (other == m_Global.Transform)
				return;
			if (m_Node.ChildrenCount)
				MarkDescendantsDirty();
		}
		else
			SetClean(m_EntityID);

		m_Global.Transform = other;

		if (m_ParentRoot)
			m_Local.Transform = m_Global.Transform;
		else
		{
			Inherit(m_Node.Parent);
			auto& parentGlobal = m_Registry->get<CTransformGlobal>(m_Node.Parent);
			m_Local.Transform = m_Global.Transform - parentGlobal.Transform;
		}
	}

	void TransformHandle::MarkDescendantsDirty()
	{
		FE_PROFILER_FUNC();

		std::queue<EntityID> toMark;
		toMark.push(m_Node.FirstChild);

		EntityID current;
		while (!toMark.empty())
		{
			current = toMark.front();
			toMark.pop();
			while (current != NullEntityID)
			{
				auto& node = m_Registry->get<CEntityNode>(current);
				if (!IsDirty(current))
				{
					toMark.push(node.FirstChild);
					SetDirty(current);
				}
				current = node.NextSibling;
			}
		}
	}

	void TransformHandle::Inherit(EntityID entityID)
	{
		FE_PROFILER_FUNC();

		if (!IsDirty(entityID))
			return;

		auto& node = m_Registry->get<CEntityNode>(entityID);
		Inherit(node.Parent);

		auto& cLocal = m_Registry->get<CTransformLocal >(entityID).Transform;
		auto& cGlobal = m_Registry->get<CTransformGlobal>(entityID).Transform;
		auto& parentGlobal = m_Registry->get<CTransformGlobal>(node.Parent).Transform;
		cGlobal = parentGlobal + cLocal;

		SetClean(entityID);
	}


}