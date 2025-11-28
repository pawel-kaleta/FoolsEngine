#include "FE_pch.h"
#include "ComponentDestructionManager.h"

namespace fe
{
	void ComponentDestructionManager::DestroyComponents(Registry& registry)
	{
		FE_PROFILER_FUNC();

		for (auto& enroll : m_Erasures)
		{
			auto& func_ptr = enroll.EraseFunkPtr;
			auto& entityID = enroll.m_EntityID;

#ifdef FE_INTERNAL_BUILD
			auto& all_of_ptr = enroll.AllOfFunkPtr;
			FE_CORE_ASSERT((registry.*all_of_ptr)(entityID), "Entity does not have this component! Use RemoveIfExist if you are unsure whether this component could have be scheduled for destruction somewhere else before.");
#endif // FE_INTERNAL_BUILD

			(registry.*func_ptr)(entityID);
		}

		m_Erasures.clear();

		for (auto& enroll : m_Removals)
		{
			auto& func_ptr = enroll.RemoveFunkPtr;
			auto& entityID = enroll.m_EntityID;
			(registry.*func_ptr)(entityID);
		}

		m_Removals.clear();
	}
}