#pragma once

#include "System.h"
#include "SystemsDirector.h"
#include "FoolsEngine/Foundation/Memory/Arena.h"

namespace fe
{
	class SystemsRegistry
	{
	public:
		static SystemsRegistry& Get() { return *s_Instance; }

		struct Item
		{
			System*	(SystemsDirector::* Create)();
			String	(*GetName)();
		};

		template <typename tnSystem>
		static void RegisterSystem()
		{
			s_Instance->m_Items.Append(
				Item{
					&SystemsDirector::CreateSystemAsBase<tnSystem>,
					&tnSystem::GetNameStatic
				}
			);
		}

		static const Item* GetItem(const std::string& name);

		DynamicArena<Item> m_Items;

	private:
		static SystemsRegistry* s_Instance;

		friend class SystemsDirector;
		friend class Application;
		SystemsRegistry() { s_Instance = this; }
		void RegisterSystems();
		void Shutdown() {};
	};
}