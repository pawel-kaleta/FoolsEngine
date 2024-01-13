#include "FE_pch.h"

#include "SystemsRegistry.h"

namespace fe
{
	SystemsRegistry SystemsRegistry::m_Instance = SystemsRegistry();

	void SystemsRegistry::RegisterSystems()
	{

	}

	const SystemsRegistry::Item* SystemsRegistry::GetItem(const std::string& name) const
	{
		for (auto& item : Items)
		{
			if (item.SystemTypeName == name)
			{
				return &item;
			}
		}

		FE_CORE_ASSERT(false, "System not found in registry");

		return nullptr;
	}
}