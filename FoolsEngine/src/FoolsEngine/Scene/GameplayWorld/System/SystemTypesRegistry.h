#pragma once

#include "System.h"
#include "SystemsDirector.h"
#include "FoolsEngine/Foundation/Containers/Arena.h"

namespace fe
{
	class SystemTypesRegistry
	{
	public:
		static SystemTypesRegistry& Get() { return *s_Instance; }

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

		static const Item* GetItem(String name);

		DynamicArena<Item> m_Items;

	private:
		static SystemTypesRegistry* s_Instance;

		friend class SystemsDirector;
		friend class Application;
		SystemTypesRegistry() { s_Instance = this; }
		void RegisterSystems();
		void Shutdown() {};
	};
}