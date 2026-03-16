#pragma once

#include "BaseEntity.h"
#include "GameplayWorld/Entity.h"

#include <vector>

namespace fe
{
	struct DataComponent;

#define FE_COMPONENT_TYPES_LIST CCamera, CTile, CSprite, CRenderMesh, CRenderMeshView, CModel, CModelView, CModelView, CDirectionalLight

	class ComponentTypesRegistry
	{
	public:
		static ComponentTypesRegistry& Get() { return *s_Instance; }

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
		
		std::vector<DataComponentRegistryItem> m_DataItems;
		std::vector<FlagComponentRegistryItem> m_FlagItems;

	private:
		static ComponentTypesRegistry* s_Instance;

		friend class Application;
		ComponentTypesRegistry() { s_Instance = this; };
		void RegisterComponents();
		void Shutdown() {};
	};
}