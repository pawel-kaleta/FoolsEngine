#pragma once

#include "System.h"
#include "SystemsDirector.h"

namespace fe
{
	class SystemsRegistry
	{
	public:
		static SystemsRegistry& Get() { return *s_Instance; }

		struct Item
		{
			System*     (SystemsDirector::* Create)();
			std::string (*GetName)();
		};

		template <typename tnSystem>
		static void RegisterSystem()
		{
			s_Instance->m_Items.push_back(
				Item{
					&SystemsDirector::CreateSystemAsBase<tnSystem>,
					&tnSystem::GetNameStatic
				}
			);
		}

		static const Item* GetItem(const std::string& name);

		std::vector<Item> m_Items;

	private:
		static SystemsRegistry* s_Instance;

		friend class SystemsDirector;
		friend class Application;
		SystemsRegistry() { s_Instance = this; }
		void RegisterSystems();
		void Shutdown() {};
	};
}