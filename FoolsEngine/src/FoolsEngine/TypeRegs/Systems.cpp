#include "Systems.h"

namespace fe::TypeRegs
{
	DynamicArena<Systems::Item> Systems::sItems;

	void Systems::Init()
	{
		sItems.Init();
	}

	void Systems::RegisterSystems()
	{
		FE_PROFILER_FUNC();

		Context::Log::Core.Warn("Register systems not implemented");
	}

	const Systems::Item* Systems::GetItem(String name)
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