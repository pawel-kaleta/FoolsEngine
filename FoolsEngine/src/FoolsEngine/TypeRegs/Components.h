#pragma once

#include "FoolsEngine/Scene/BaseEntity.h"
#include "FoolsEngine/Scene/GameplayWorld/Entity.h"

namespace fe
{
	struct DataComponent;
}

namespace fe::TypeRegs
{

#define FE_COMPONENT_TYPES_LIST CCamera, CTile, CSprite, CRenderMesh, CRenderMeshView, CModel, CModelView, CModelView, CDirectionalLight

	class Components
	{
	public:
		struct DataComponentRegistryItem
		{
			DataComponent *	(BaseEntity::* Getter)() const;
			void			(BaseEntity::* Emplacer)() const;
			void			(Entity::* DestructionScheduler)();
			String			Name;
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
			sDataItems.Append(
				DataComponentRegistryItem{
					&BaseEntity::GetAsDataComponentIfExist<tnComponent>,
					&BaseEntity::DefaultEmplace<tnComponent>,
					&Entity::RemoveIfExist<tnComponent>,
					tnComponent::GetNameStatic()
				}
			);
		}

		template <typename tnFlagComponent>
		static void RegisterFlagComponent()
		{
			sFlagItems.Append(
				FlagComponentRegistryItem{
					&BaseEntity::AllOf<tnFlagComponent>,
					&BaseEntity::Flag<tnFlagComponent>,
					&BaseEntity::UnFlag<tnFlagComponent>
				}
			);
		}
		
		static DynamicArena<DataComponentRegistryItem> sDataItems;
		static DynamicArena<FlagComponentRegistryItem> sFlagItems;

		static void Init();
		static void RegisterComponents();

		static void Shutdown() {};
	};
}