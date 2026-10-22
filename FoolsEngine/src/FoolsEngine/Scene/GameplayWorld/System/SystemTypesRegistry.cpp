#include "SystemTypesRegistry.h"

namespace fe
{
	DynamicArena<SystemTypesRegistry::Item> SystemTypesRegistry::sItems;

	void SystemTypesRegistry::Init()
	{
		sItems.Init();
	}

	void SystemTypesRegistry::RegisterSystems()
	{
		FE_PROFILER_FUNC();

		Context::Log::Core.Warn("Register systems not implemented");
	}

	const SystemTypesRegistry::Item* SystemTypesRegistry::GetItem(String name)
	{
		FE_PROFILER_FUNC();

		for (auto& item : sItems)
		{
			if (CompareStringsEqual(item.Name, name))
			{
				return &item;
			}
		}

		return nullptr;
	}
}