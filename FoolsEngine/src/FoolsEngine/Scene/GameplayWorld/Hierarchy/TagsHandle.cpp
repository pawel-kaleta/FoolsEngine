#include "FE_pch.h"
#include "TagsHandle.h"

#include <queue>

namespace fe
{
	TagsHandle::TagsHandle(EntityID ID, Registry* registry)
		:
		m_CTags(registry->get<CTags>(ID)),
		m_Node(registry->get<CEntityNode>(ID)),
		m_EntityID(ID),
		m_Registry(registry)
	{
		FE_CORE_ASSERT(m_Node.Parent != NullEntityID, "Node does not have a parent!");
	}

	void TagsHandle::SetLocal(const Tags& other)
	{
		if (other == m_CTags.Local)
			return;

		m_CTags.Local = other;

		UpdateTags();
	}

	void TagsHandle::UpdateTags()
	{
		FE_PROFILER_FUNC();

		auto& node_storage = m_Registry->storage<CEntityNode>();
		auto& tags_storage = m_Registry->storage<CTags>();

		std::queue<EntityID> entities_to_update;
		entities_to_update.push(m_EntityID);

		EntityID current;
		EntityID first_sibling;
		do
		{
			first_sibling = entities_to_update.front();
			entities_to_update.pop();

			auto& first_sibling_node = node_storage.get(first_sibling);
			auto& parent_tags = tags_storage.get(first_sibling_node.Parent);

			current = first_sibling;
			do
			{
				auto& tags = tags_storage.get(current);
				tags.Global = parent_tags.Global + tags.Local;

				auto& current_node = node_storage.get(current);

				if (current_node.ChildrenCount)
					entities_to_update.push(current_node.FirstChild);

				current = current_node.NextSibling;
			} while (current != first_sibling && current != NullEntityID);

		} while (!entities_to_update.empty());
	}
}