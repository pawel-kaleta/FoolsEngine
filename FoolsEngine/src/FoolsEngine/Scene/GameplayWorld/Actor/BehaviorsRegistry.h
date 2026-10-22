#pragma once

#include "Actor.h"
#include "Behavior.h"

#include "FoolsEngine/Foundation/Containers/Arena.h"

namespace fe
{
	class BehaviorsRegistry
	{
	public:
		struct Item
		{
			Behavior*	(Actor::* Create)();
			String		Name;
		};

		template <typename tnBehavior>
		static void RegisterBehavior()
		{
			sItems.Append(
				Item{
					&Actor::CreateBehaviorAsBase<tnBehavior>,
					tnBehavior::GetNameStatic()
				}
			);
		}

		static const Item* GetItemFromName(String name);
		
		static DynamicArena<Item> sItems;

		static void Init();
		static void RegisterBehaviors();
		static void Shutdown() {};
	};
}