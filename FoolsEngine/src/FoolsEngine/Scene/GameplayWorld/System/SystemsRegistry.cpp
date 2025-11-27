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
			auto& nameFunkPtr = item.GetName;
			if ((*nameFunkPtr)().compare(name) == 0)
			{
				return &item;
			}
		}

		//FE_CORE_ASSERT(false, "System not found in registry");

		return nullptr;
	}
}