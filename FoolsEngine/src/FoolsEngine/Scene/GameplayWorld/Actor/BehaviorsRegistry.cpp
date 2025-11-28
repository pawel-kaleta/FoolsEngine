#include "FE_pch.h"
#include "BehaviorsRegistry.h"

namespace fe
{
	BehaviorsRegistry* BehaviorsRegistry::s_Instance;

	void BehaviorsRegistry::RegisterBehaviors()
	{
		FE_PROFILER_FUNC();
	}

	const BehaviorsRegistry::Item* BehaviorsRegistry::GetItemFromName(const std::string& name)
	{
		FE_PROFILER_FUNC();

		for (auto& item : s_Instance->m_Items)
		{
			auto& name_funk_ptr = item.GetName;

			auto current_behavior_name = (*name_funk_ptr)();
			if (current_behavior_name == name)
			{
				return &item;
			}
		}

		FE_CORE_ASSERT(false, "Behavior not found in registry");

		return nullptr;
	}
}