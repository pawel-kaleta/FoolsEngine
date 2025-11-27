#pragma once

#include "FoolsEngine\Scene\ECS.h"
#include "FoolsEngine\Scene\Component.h"

namespace fe
{
	inline std::vector<EntityID> ChildrenList(EntityID parentID, Registry& registry)
	{
		auto& nodeStorage = registry.storage<CEntityNode>();
		auto& parentNode = nodeStorage.get(parentID);

		std::vector<EntityID> result(parentNode.ChildrenCount);

		EntityID currentChild = parentNode.FirstChild;
		for (auto it = result.begin(); it < result.end(); ++it)
		{
			*it = currentChild;
			auto& node = nodeStorage.get(currentChild);
			currentChild = node.NextSibling;
		}

		return result;
	}
}