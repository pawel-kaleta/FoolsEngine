#include "SystemTypesRegistry.h"

namespace fe
{
	SystemTypesRegistry* SystemTypesRegistry::s_Instance;

	void SystemTypesRegistry::RegisterSystems()
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(false, "Not implemented");
	}

	const SystemTypesRegistry::Item* SystemTypesRegistry::GetItem(String name)
	{
		FE_PROFILER_FUNC();

		for (auto& item : s_Instance->m_Items)
		{
			auto name_form_item = (*item.GetName)();
			if (CompareStringsEqual(name_form_item, name))
			{
				return &item;
			}
		}

		return nullptr;
	}
}