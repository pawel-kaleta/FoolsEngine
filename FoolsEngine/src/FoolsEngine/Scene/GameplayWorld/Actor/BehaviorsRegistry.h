#pragma once

#include "Actor.h"
#include "Behavior.h"

#include "FoolsEngine/Foundation/Containers/Arena.h"

namespace fe
{
	class BehaviorsRegistry
	{
	public:
		static BehaviorsRegistry& Get() { return *s_Instance; }

		struct Item
		{
			Behavior*	(Actor::* Create)();
			String		(* GetName)();
		};

		template <typename tnBehavior>
		static void RegisterBehavior()
		{
			s_Instance->m_Items.Append(
				Item{
					&Actor::CreateBehaviorAsBase<tnBehavior>,
					&tnBehavior::GetNameStatic
				}
			);
		}

		static const Item* GetItemFromName(String name);
		
		DynamicArena<Item> m_Items;
	
	private:
		static BehaviorsRegistry* s_Instance;

		friend class Application;
		BehaviorsRegistry() { s_Instance = this;}
		void RegisterBehaviors();
		void Shutdown() {};
	};
}