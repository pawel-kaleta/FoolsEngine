#include "FE_pch.h"
#include "BehaviorsRegistry.h"

namespace fe
{
	BehaviorsRegistry* BehaviorsRegistry::s_Instance;

	void BehaviorsRegistry::RegisterBehaviors()
	{
		FE_PROFILER_FUNC();
	}

	const BehaviorsRegistry::Item* BehaviorsRegistry::GetItemFromNameInternal(const std::string& name) const
	{
		FE_PROFILER_FUNC();

		for (auto& item : m_Items)
		{
			auto& nameFunkPtr = item.GetName;

			auto string1 = (*nameFunkPtr)();
			if (string1.compare(name) == 0)
			{
				return &item;
			}
		}

		FE_CORE_ASSERT(false, "Behavior not found in registry");

		return nullptr;
	}
}