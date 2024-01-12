#pragma once

#include "Behavior.h"
#include "Actor.h"

namespace fe
{
	class BehaviorsRegistry
	{
	public:
		struct BehaviorsRegistryItem
		{
			Behavior*   (Actor::* Create)();
			std::string (BehaviorsRegistry::* Name)();
		};
		std::vector<BehaviorsRegistryItem> Items;

		static BehaviorsRegistry s_Registry;

		void RegisterBehaviors();

		template <typename tnBehavior>
		void RegisterBehavior()
		{
			Items.push_back(
				BehaviorsRegistryItem{
					&Actor::CreateBehaviorAsBase<tnBehavior>,
					&BehaviorsRegistry::GetName<tnBehavior>
				}
			);
		}

	private:
		template <typename tnBehavior>
		std::string GetName()
		{
			return tnBehavior::GetName();
		}
	};
}