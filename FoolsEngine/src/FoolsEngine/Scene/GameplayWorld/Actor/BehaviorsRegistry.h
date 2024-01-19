#pragma once

#include "Behavior.h"
#include "Actor.h"

namespace fe
{
	class BehaviorsRegistry
	{
	public:
		static BehaviorsRegistry& GetInstance() { return s_Instance; }

		struct Item
		{
			Behavior*   (Actor::* Create)();
			std::string (BehaviorsRegistry::* Name)();
		};

		void RegisterBehaviors();

		template <typename tnBehavior>
		void RegisterBehavior()
		{
			Items.push_back(
				Item{
					&Actor::CreateBehaviorAsBase<tnBehavior>,
					&BehaviorsRegistry::GetName<tnBehavior>
				}
			);
		}

		const Item* GetItemFromName(const std::string& name);

	private:
		friend class ActorInspector;

		BehaviorsRegistry() = default;
		static BehaviorsRegistry s_Instance;
		
		std::vector<Item> Items;


		template <typename tnBehavior>
		std::string GetName()
		{
			return tnBehavior::GetName();
		}
	};
}