#pragma once

#include "FoolsEngine/Scene/ECS.h"
#include "FoolsEngine/Foundation/Memory/Arena.h"

namespace fe
{
	class ComponentDestructionManager
	{
	public:
		template <typename tnComponent>
		void ScheduleRemoval(EntityID entityID)
		{
			m_Removals.Append( RemovalEnroll{ &Registry::remove<tnComponent>, entityID } );
		}

		template <typename tnComponent>
		void ScheduleErasure(EntityID entityID)
		{
			m_Erasures.Append( ErasureEnroll{
#ifdef FE_INTERNAL_BUILD
				&Registry::all_of<tnComponent>,
#endif // FE_INTERNAL_BUILD
				&Registry::erase<tnComponent>,
				entityID } );
		}

		void DestroyComponents(Registry& registry);
	private:
		struct RemovalEnroll
		{
			std::size_t (Registry::* RemoveFunkPtr)(EntityID);
			EntityID m_EntityID;
		};
		DynamicArena<RemovalEnroll> m_Removals;

		struct ErasureEnroll
		{
#ifdef FE_INTERNAL_BUILD
			bool (Registry::* AllOfFunkPtr)(EntityID);
#endif // FE_INTERNAL_BUILD
			void (Registry::* EraseFunkPtr)(EntityID);
			EntityID m_EntityID;
		};
		DynamicArena<ErasureEnroll> m_Erasures;
	};
}
