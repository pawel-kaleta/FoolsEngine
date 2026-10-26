#include "Behaviors.h"

namespace fe::TypeRegs
{
	DynamicArena<Behaviors::Item> Behaviors::sItems;

	void Behaviors::Init()
	{
		sItems.Init();
	}

	void Behaviors::RegisterBehaviors()
	{
		FE_PROFILER_FUNC();
	}

	const Behaviors::Item* Behaviors::GetItemFromName(String name)
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