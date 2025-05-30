#pragma once

#include "BaseEntity.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"

#include <vector>

namespace fe
{
	struct DataComponent;

	class ComponentTypesRegistry
	{
	public:
		struct DataComponentRegistryItem
		{
			DataComponent* (BaseEntity::* Getter)() const;
			void           (BaseEntity::* Emplacer)() const;
			void           (Entity::* DestructionScheduler)();
			std::string    (*GetName)();
		};

		struct FlagComponentRegistryItem
		{
			bool (BaseEntity::* Checker)();
			void (BaseEntity::* Flagger)();
			void (BaseEntity::* Remover)();
		};

		template <typename tnComponent>
		static void RegisterDataComponent()
		{
			s_Instance->m_DataItems.push_back(
				DataComponentRegistryItem{
					&BaseEntity::GetAsDataComponentIfExist<tnComponent>,
					&BaseEntity::DefaultEmplace<tnComponent>,
					&Entity::RemoveIfExist<tnComponent>,
					&tnComponent::GetNameStatic
				}
			);
		}

		template <typename tnFlagComponent>
		static void RegisterFlagComponent()
		{
			s_Instance->m_FlagItems.push_back(
				FlagComponentRegistryItem{
					&BaseEntity::AllOf<tnFlagComponent>,
					&BaseEntity::Flag<tnFlagComponent>,
					&BaseEntity::UnFlag<tnFlagComponent>
				}
			);
		}

		static const std::vector<const DataComponentRegistryItem>& GetDataCompItems() { return s_Instance->m_DataItems; };
		static const std::vector<const FlagComponentRegistryItem>& GetFlagCompItems() { return s_Instance->m_FlagItems; };
		
		
	private:
		friend class Application;
		ComponentTypesRegistry() { s_Instance = this; };
		void RegisterComponents();
		void Shutdown() {};

		static ComponentTypesRegistry* s_Instance;

		std::vector<const DataComponentRegistryItem> m_DataItems;
		std::vector<const FlagComponentRegistryItem> m_FlagItems;

	};
}