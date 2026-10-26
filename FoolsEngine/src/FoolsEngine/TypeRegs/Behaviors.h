#pragma once

#include "FoolsEngine/Scene/GameplayWorld/Actor/Actor.h"
#include "FoolsEngine/Scene/GameplayWorld/Actor/Behavior.h"

#include "FoolsEngine/Foundation/Containers/Arena.h"

namespace fe::TypeRegs
{
	class Behaviors
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
			auto* item = sItems.EmplaceBack();
			item->Create = &Actor::CreateBehaviorAsBase<tnBehavior>;
			item->Name = tnBehavior::GetNameStatic();
		}

		static const Item* GetItemFromName(String name);
		
		static DynamicArena<Item> sItems;

		static void Init();
		static void RegisterBehaviors();
		static void Shutdown() {};
	};
}