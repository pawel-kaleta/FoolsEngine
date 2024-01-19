#pragma once

#include "FoolsEngine\Scene\Component.h"
#include "Tags.h"

#include <queue>

namespace fe
{
	class TagsHandle
	{
	public:
		TagsHandle(EntityID ID, Registry* registry);

		const Tags& GetLocal()  const { return m_CTags.Local;  }
		const Tags& GetGlobal()	const {	return m_CTags.Global; }

		Tags Local()  const { return m_CTags.Local; }
		Tags Global() const { return m_CTags.Global; }

		operator const Tags& () { return m_CTags.Global; }
		operator       Tags  () { return m_CTags.Global; }

		void SetLocal(const Tags& other);

		bool Contains(Tags tags)         const { return m_CTags.Global & tags; }
		bool Contains(Tags::TagList tag) const { return Contains((Tags)tag); }

		void Add(Tags tags)		{ SetLocal(m_CTags.Local + tags); }
		void Remove(Tags tags)	{ SetLocal(m_CTags.Local - tags); }

		void Add(Tags::TagList tag)		{ Add((Tags)tag); }
		void Remove(Tags::TagList tag)	{ Remove((Tags)tag); }

	private:
		friend class HierarchyDirector;
		friend class SceneSerializerYAML;

		CTags&			m_CTags;
		CEntityNode&	m_Node;
		Registry*		m_Registry;
		EntityID		m_EntityID;

		void UpdateTags();
	};

}