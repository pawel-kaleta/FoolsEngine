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
		static ComponentTypesRegistry& GetInstance() { return s_Registry; }

		struct DataComponentRegistryItem
		{
			DataComponent* (BaseEntity::* Getter)() const;
			void           (BaseEntity::* Emplacer)() const;
			void           (Entity::* DestructionScheduler)();
			std::string    (ComponentTypesRegistry::* Name)();
		};
		std::vector<DataComponentRegistryItem> DataItems;

		struct FlagComponentRegistryItem
		{
			bool (BaseEntity::* Checker)();
			void (BaseEntity::* Flagger)();
			void (BaseEntity::* Remover)();
		};
		std::vector<FlagComponentRegistryItem> FlagItems;

		void RegisterComponents();

		template <typename tnComponent>
		void RegisterDataComponent()
		{
			DataItems.push_back(
				DataComponentRegistryItem{
					&BaseEntity::GetAsDataComponentIfExist<tnComponent>,
					&BaseEntity::DefaultEmplace<tnComponent>,
					&Entity::RemoveIfExist<tnComponent>,
					&ComponentTypesRegistry::GetName<tnComponent>
				}
			);
		}

		template <typename tnFlagComponent>
		void RegisterFlagComponent()
		{
			FlagItems.push_back(
				FlagComponentRegistryItem{
					&BaseEntity::AllOf<tnFlagComponent>,
					&BaseEntity::Flag<tnFlagComponent>,
					&BaseEntity::UnFlag<tnFlagComponent>
				}
			);
		}

	private:
		ComponentTypesRegistry() {};
		static ComponentTypesRegistry s_Registry;

		template <typename tnComponent>
		std::string GetName()
		{
			return tnComponent::GetName();
		}
	};
}