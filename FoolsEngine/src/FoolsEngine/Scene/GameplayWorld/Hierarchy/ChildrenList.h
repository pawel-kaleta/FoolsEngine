#pragma once

#include "FoolsEngine/Scene/ECS.h"
#include "FoolsEngine/Scene/Component.h"

#include "FoolsEngine/Foundation/Memory/Arena.h"
#include "FoolsEngine/Foundation/Utils/Context.h"


namespace fe
{
	template <typename tAlloc>
	Splice<EntityID> ChildrenList(EntityID parentID, Registry& registry, tAlloc* alloc = Context::Allocators::Output)
	{
		auto& nodeStorage = registry.storage<CEntityNode>();
		auto& parentNode = nodeStorage.get(parentID);

		Splice<EntityID> result = ((TypedAlloc<tAlloc>*)alloc)->Allocate<EntityID>(parentNode.ChildrenCount);

		EntityID currentChild = parentNode.FirstChild;
		for (auto& result_child : result)
		{
			result_child = currentChild;
			auto& node = nodeStorage.get(currentChild);
			currentChild = node.NextSibling;
		}

		return result;
	}
}