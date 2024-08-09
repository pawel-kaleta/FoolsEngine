#pragma once

#include "System.h"

#include "SystemsDirector.h"

namespace fe
{
	class SystemsRegistry
	{
	public:
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

		static const Item* GetItem(const std::string& systemTypeName) { return s_Instance->GetItemInternal(systemTypeName); };

		static std::vector<Item>& GetAll() { return s_Instance->m_Items; }
	private:
		friend class SystemsDirector;
		friend class Application;
		SystemsRegistry() { s_Instance = this; }
		void RegisterSystems();
		void Shutdown() {};

		static SystemsRegistry* s_Instance;

		std::vector<Item> m_Items;
		const Item* GetItemInternal(const std::string& systemTypeName) const;
	};
}