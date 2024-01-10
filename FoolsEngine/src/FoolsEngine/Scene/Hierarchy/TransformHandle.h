#pragma once

#include "FoolsEngine\Scene\Component.h"

#include <queue>

namespace fe
{
	// provides safe interface to operate on either Local or Global Transform Component
	// Overloaded operators allowing to treat it as if it was Global Transform Component itself
	// 
	// Local recalculated upon change, Global recalculated upon access if needed (has changed)
	// Local is always accurate, Global appears as always accurate
	//
	// DirtyFlag component Marks Global part as „outdated”
	// Emplaced in all descendant Entities upon Global modification (Local modification implies Global modification)
	// Global is recalculated upon access if necessary (if marked as „dirty”) using hierarchy chain starting from closest „clean” ancestor

	class TransformHandle
	{
	public:
		TransformHandle(EntityID ID, Registry* registry);

		const Transform& GetLocal() const { return m_Local.Transform; }
		const Transform& GetGlobal()
		{
			if (!m_ParentRoot)
				Inherit(m_EntityID);
			return m_Global.Transform;
		}

		Transform Local()  const { return m_Local.Transform; }
		Transform Global()       { return GetGlobal(); }

		operator const Transform& () { return GetGlobal(); }
		operator       Transform  () { return GetGlobal(); }

		void operator= (const Transform& other) { SetGlobal(other); }

		void SetLocal(const Transform& other);
		void SetGlobal(const Transform& other);

	private:
		CTransformLocal&	m_Local;
		CTransformGlobal&	m_Global;
		CEntityNode&		m_Node;
		Registry*			m_Registry;
		EntityID			m_EntityID;
		bool				m_ParentRoot = false;

		bool IsDirty(EntityID entityID) const { return m_Registry->all_of<CDirtyFlag<CTransformGlobal>>(entityID); }
		bool IsDirty()                  const { return IsDirty(m_EntityID); }

		void SetDirty(EntityID entityID) { m_Registry->emplace<CDirtyFlag<CTransformGlobal>>(entityID); }
		void SetClean(EntityID entityID) { m_Registry->erase  <CDirtyFlag<CTransformGlobal>>(entityID); }

		void MarkDescendantsDirty();
		void Inherit(EntityID entityID);
	};
}