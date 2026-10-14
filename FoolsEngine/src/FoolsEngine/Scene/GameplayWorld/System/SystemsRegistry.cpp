#include "SystemsRegistry.h"

namespace fe
{
	SystemsRegistry* SystemsRegistry::s_Instance;

	void SystemsRegistry::RegisterSystems()
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(false, "Not implemented");
	}

	const SystemsRegistry::Item* SystemsRegistry::GetItem(const std::string& name)
	{
		FE_PROFILER_FUNC();

		for (auto& item : s_Instance->m_Items)
		{
			auto name_form_item = (*item.GetName)();
			if (CompareStringsEqual(name_form_item, String(name.c_str())))
			{
				return &item;
			}
		}

		//FE_CORE_ASSERT(false, "System not found in registry");

		return nullptr;
	}
}