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
			auto& nameFunkPtr = item.Name;
			if ((this->*nameFunkPtr)().compare(name) == 0)
			{
				return &item;
			}
		}

		//FE_CORE_ASSERT(false, "System not found in registry");

		return nullptr;
	}
}