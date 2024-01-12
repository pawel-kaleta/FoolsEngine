#pragma once

#include "FoolsEngine\Scene\ECS.h"
#include "FoolsEngine\Scene\Component.h"

#include <vector>

namespace fe
{
	class ChildrenList
	{
	public:
		ChildrenList(ChildrenList&) = default;
		ChildrenList(EntityID parentID, Registry* registry)
		{
			auto& nodeStorage = registry->storage<CEntityNode>();

			auto& parentNode = nodeStorage.get(parentID);
			m_Children = std::vector<EntityID>(parentNode.ChildrenCount);

			EntityID currentChild = parentNode.FirstChild;
			for (auto it = m_Children.begin(); it < m_Children.end(); ++it)
			{
				*it = currentChild;
				auto& node = nodeStorage.get(currentChild);
				currentChild = node.NextSibling;
			}
		}

		std::vector<EntityID>::iterator Begin() { return m_Children.begin(); }
		std::vector<EntityID>::iterator End() { return m_Children.end(); }

		uint32_t Count() const { return (uint32_t)m_Children.size(); }

	private:
		std::vector<EntityID> m_Children;
	};
}