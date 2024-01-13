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
			System* (SystemsDirector::* Create)();
			std::string SystemTypeName;
		};
		std::vector<Item> Items;

		void RegisterSystems();

		template <typename tnSystem>
		void RegisterSystem(const std::string& systemTypeName)
		{
			Items.push_back(
				Item{
					&SystemsDirector::CreateSystemAsBase<tnSystem>,
					systemTypeName
				}
			);
		}

		const Item* GetItem(const std::string& systemTypeName) const;

		static SystemsRegistry& GetInstance() { return m_Instance; }
	private:
		friend class SystemsDirector;
		static SystemsRegistry m_Instance;
	};
}