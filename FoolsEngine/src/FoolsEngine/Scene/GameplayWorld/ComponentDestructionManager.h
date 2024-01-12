#pragma once

#include "FoolsEngine\Scene\ECS.h"

namespace fe
{
	class ComponentDestructionManager
	{
	public:
		template <typename tnComponent>
		void ScheduleRemoval(EntityID entityID)
		{
			m_Removals.push_back( RemovalEnroll{ &Registry::remove<tnComponent>, entityID } );
		}

		template <typename tnComponent>
		void ScheduleErasure(EntityID entityID)
		{
			m_Erasures.push_back( ErasureEnroll{ &Registry::erase<tnComponent>,	entityID } );
		}

		void DestroyComponents(Registry& registry)
		{
			FE_PROFILER_FUNC();

			for (auto& enroll : m_Erasures)
			{
				auto& funcPtr = enroll.EraseFunkPtr;
				auto& entityID = enroll.m_EntityID;
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
	private:
		struct RemovalEnroll
		{
			std::size_t (Registry::* RemoveFunkPtr)(EntityID);
			EntityID m_EntityID;
		};
		std::vector<RemovalEnroll> m_Removals;

		struct ErasureEnroll
		{
			void (Registry::* EraseFunkPtr)(EntityID);
			EntityID m_EntityID;
		};
		std::vector<ErasureEnroll> m_Erasures;
	};
}
