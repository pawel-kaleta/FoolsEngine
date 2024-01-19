#include "FE_pch.h"

#include "BehaviorsRegistry.h"

namespace fe
{
	BehaviorsRegistry BehaviorsRegistry::s_Instance = BehaviorsRegistry();

	void BehaviorsRegistry::RegisterBehaviors()
	{

	}

	const BehaviorsRegistry::Item* BehaviorsRegistry::GetItemFromName(const std::string& name)
	{
		for (auto& item : Items)
		{
			auto& nameFunkPtr = item.Name;

			auto& string1 = (this->*nameFunkPtr)();
			if (string1.compare(name) == 0)
			{
				return &item;
			}
		}

		FE_CORE_ASSERT(false, "Behavior not found in registry");

		return nullptr;
	}
}