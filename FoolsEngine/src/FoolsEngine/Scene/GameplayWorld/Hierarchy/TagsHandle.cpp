#include "FE_pch.h"

#include "TagsHandle.h"

namespace fe
{
	TagsHandle::TagsHandle(EntityID ID, Registry* registry)
		:
		m_CTags(registry->get<CTags>(ID)),
		m_Node(registry->get<CEntityNode>(ID)),
		m_EntityID(ID),
		m_Registry(registry)
	{
		//FE_CORE_ASSERT(m_Node.Parent != NullEntityID, "Node does not have a parent!");
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

		auto& nodeStorage = m_Registry->storage<CEntityNode>();
		auto& tagsStorage = m_Registry->storage<CTags>();

		std::queue<EntityID> toUpdate;
		toUpdate.push(m_EntityID);

		EntityID current;
		EntityID firstSibling;
		do
		{
			firstSibling = toUpdate.front();
			toUpdate.pop();

			auto& firstSiblingNode = nodeStorage.get(firstSibling);
			auto& parentCTags = tagsStorage.get(firstSiblingNode.Parent);

			current = firstSibling;
			do
			{
				auto& cTags = tagsStorage.get(current);
				cTags.Global = parentCTags.Global + cTags.Local;

				auto& currentNode = nodeStorage.get(current);

				if (currentNode.ChildrenCount)
					toUpdate.push(currentNode.FirstChild);

				current = currentNode.NextSibling;
			} while (current != firstSibling && current != NullEntityID);

		} while (!toUpdate.empty());
	}
}