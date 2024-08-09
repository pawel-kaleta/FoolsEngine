#pragma once

#include "Behavior.h"
#include "Actor.h"

namespace fe
{
	class BehaviorsRegistry
	{
	public:
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

		static const Item* GetItemFromName(const std::string& name) { return s_Instance->GetItemFromNameInternal(name); }
		static std::vector<Item>& GetAll() { return s_Instance->m_Items; }
	private:
		friend class ActorInspector;
		friend class Application;
		BehaviorsRegistry() { s_Instance = this;}
		void RegisterBehaviors();
		void Shutdown() {};

		static BehaviorsRegistry* s_Instance;

		std::vector<Item> m_Items;
		const Item* GetItemFromNameInternal(const std::string& name) const;
	};
}