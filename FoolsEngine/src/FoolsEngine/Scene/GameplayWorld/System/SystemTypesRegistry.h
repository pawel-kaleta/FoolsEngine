#pragma once

#include "System.h"
#include "SystemsDirector.h"
#include "FoolsEngine/Foundation/Containers/Arena.h"

namespace fe
{
	class SystemTypesRegistry
	{
	public:
		struct Item
		{
			System*	(SystemsDirector::* Create)();
			String	Name;
		};

		template <typename tnSystem>
		static void RegisterSystem()
		{
			sItems.Append(
				Item{
					&SystemsDirector::CreateSystemAsBase<tnSystem>,
					tnSystem::GetNameStatic()
				}
			);
		}

		static const Item* GetItem(String name);

		static DynamicArena<Item> sItems;

		static void Init();
		static void RegisterSystems();
		static void Shutdown() {};
	};
}