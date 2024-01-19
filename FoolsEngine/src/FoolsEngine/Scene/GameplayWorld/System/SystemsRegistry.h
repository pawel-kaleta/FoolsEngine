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
			std::string (SystemsRegistry::* Name)() const;
		};
		std::vector<Item> Items;

		void RegisterSystems();

		template <typename tnSystem>
		void RegisterSystem()
		{
			Items.push_back(
				Item{
					&SystemsDirector::CreateSystemAsBase<tnSystem>,
					&SystemsRegistry::GetName<tnSystem>
				}
			);
		}

		const Item* GetItem(const std::string& systemTypeName) const;

		static SystemsRegistry& GetInstance() { return m_Instance; }
	private:
		friend class SystemsDirector;
		static SystemsRegistry m_Instance;

		template <typename tnSystem>
		std::string GetName() const
		{
			return tnSystem::GetName();
		}
	};
}