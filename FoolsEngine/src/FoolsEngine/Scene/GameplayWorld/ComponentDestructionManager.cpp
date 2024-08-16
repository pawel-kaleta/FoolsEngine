#include "FE_pch.h"
#include "ComponentDestructionManager.h"

namespace fe
{
	void ComponentDestructionManager::DestroyComponents(Registry& registry)
	{
		FE_PROFILER_FUNC();

		for (auto& enroll : m_Erasures)
		{
			auto& funcPtr = enroll.EraseFunkPtr;
			auto& entityID = enroll.m_EntityID;

#ifdef FE_INTERNAL_BUILD
			auto& allOfPtr = enroll.AllOfFunkPtr;
			FE_CORE_ASSERT((registry.*allOfPtr)(entityID), "Entity does not have this component! Use RemoveIfExist if you are unsure whether this component could have be scheduled for destruction somewhere else before.");
#endif // FE_INTERNAL_BUILD

			(registry.*funcPtr)(entityID);
		}

		m_Erasures.clear();

		for (auto& enroll : m_Removals)
		{
			auto& funcPtr = enroll.RemoveFunkPtr;
			auto& entityID = enroll.m_EntityID;
			(registry.*funcPtr)(entityID);
		}

		m_Removals.clear();
	}
}