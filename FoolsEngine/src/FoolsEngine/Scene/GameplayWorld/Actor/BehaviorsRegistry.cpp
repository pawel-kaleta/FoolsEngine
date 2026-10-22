#include "BehaviorsRegistry.h"

namespace fe
{
	DynamicArena<BehaviorsRegistry::Item> BehaviorsRegistry::sItems;

	void BehaviorsRegistry::Init()
	{
		sItems.Init();
	}

	void BehaviorsRegistry::RegisterBehaviors()
	{
		FE_PROFILER_FUNC();
	}

	const BehaviorsRegistry::Item* BehaviorsRegistry::GetItemFromName(String name)
	{
		FE_PROFILER_FUNC();

		for (auto& item : sItems)
		{
			if (CompareStringsEqual(item.Name, name))
			{
				return &item;
			}
		}

		FE_CORE_ASSERT(false, "Behavior not found in registry");

		return nullptr;
	}
}