#pragma once

#include "Behavior.h"
#include "Actor.h"

namespace fe
{
	class BehaviorsRegistry
	{
	public:
		static BehaviorsRegistry& GetInstance() { return s_Instance; }

		struct BehaviorsRegistryItem
		{
			Behavior*   (Actor::* Create)();
			std::string (BehaviorsRegistry::* Name)();
		};
		std::vector<BehaviorsRegistryItem> Items;

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
		BehaviorsRegistry() = default;
		static BehaviorsRegistry s_Instance;

		template <typename tnBehavior>
		std::string GetName()
		{
			return tnBehavior::GetName();
		}
	};
}