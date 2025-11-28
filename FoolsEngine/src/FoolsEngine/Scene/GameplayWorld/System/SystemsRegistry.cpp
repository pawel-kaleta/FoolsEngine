#include "FE_pch.h"

#include "SystemsRegistry.h"

namespace fe
{
	SystemsRegistry* SystemsRegistry::s_Instance;

	void SystemsRegistry::RegisterSystems()
	{
		FE_PROFILER_FUNC();
	}

	const SystemsRegistry::Item* SystemsRegistry::GetItem(const std::string& name)
	{
		FE_PROFILER_FUNC();

		for (auto& item : s_Instance->m_Items)
		{
			auto& get_name_funk_ptr = item.GetName;
			if ((*get_name_funk_ptr)() == name)
			{
				return &item;
			}
		}

		//FE_CORE_ASSERT(false, "System not found in registry");

		return nullptr;
	}
}