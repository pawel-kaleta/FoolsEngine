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
			Get().m_Items.push_back(
				Item{
					&SystemsDirector::CreateSystemAsBase<tnSystem>,
					&tnSystem::GetNameStatic
				}
			);
		}

		static const Item* GetItem(const std::string& systemTypeName) { return Get().GetItemInternal(systemTypeName); };

		static std::vector<Item>& GetAll() { return Get().m_Items; }
	private:
		friend class SystemsDirector;
		friend class Application;
		SystemsRegistry() { m_Instance = this; }
		void RegisterSystems();
		void Shutdown() {};

		static SystemsRegistry* m_Instance;
		static SystemsRegistry& Get() { return *m_Instance; }

		std::vector<Item> m_Items;
		const Item* GetItemInternal(const std::string& systemTypeName) const;

		//template <typename tnSystem>
		//std::string GetName() const
		//{
		//	return tnSystem::GetNameStatic();
		//}
	};
}