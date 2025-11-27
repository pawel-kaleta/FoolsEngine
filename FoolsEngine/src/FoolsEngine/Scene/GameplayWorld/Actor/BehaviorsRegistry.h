#pragma once

#include "Behavior.h"
#include "Actor.h"

namespace fe
{
	class BehaviorsRegistry
	{
	public:
		static BehaviorsRegistry& Get() { return *s_Instance; }

		struct Item
		{
			Behavior*   (Actor::* Create)();
			std::string (* GetName)();
		};

		template <typename tnBehavior>
		static void RegisterBehavior()
		{
			s_Instance->m_Items.push_back(
				Item{
					&Actor::CreateBehaviorAsBase<tnBehavior>,
					&tnBehavior::GetNameStatic
				}
			);
		}

		static const Item* GetItemFromName(const std::string& name);
		
		std::vector<Item> m_Items;
	
	private:
		static BehaviorsRegistry* s_Instance;

		friend class Application;
		BehaviorsRegistry() { s_Instance = this;}
		void RegisterBehaviors();
		void Shutdown() {};
	};
}