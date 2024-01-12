#pragma once
#include "FoolsEngine\Debug\Asserts.h"
#include "ECS.h"
#include "Component.h"


#include <queue>

namespace fe
{
	class World;

	class BaseEntity
	{
	public:
		BaseEntity(EntityID entityID, World* world);
		BaseEntity(const BaseEntity& other) = default;
		BaseEntity() = default;

		//all pools :)
		auto Storages() const
		{
			return m_Handle.storage();
		}

		EntityID ID()		const { return m_Handle.entity(); }
		operator EntityID()	const { return ID(); }
		operator bool()		const { return m_Handle.operator bool() && m_World; }

		// all pools :)
		bool IsEmpty() { return m_Handle.orphan(); }

		//In global storage pool only!
		template<typename... tnComponents>
		bool AllOf() { return m_Handle.all_of<tnComponents...>(); }

		//In global storage pool only!
		template<typename... tnComponents>
		bool AnyOf() { return m_Handle.any_of<tnComponents...>(); }

		//In global storage pool only!
		template<typename tnComponent, typename... Args>
		tnComponent& Emplace(Args&&... args)
		{
			static_assert(std::is_base_of_v<DataComponent, tnComponent>, "This is not a component!");
			
			FE_CORE_ASSERT(!AnyOf<tnComponent>(), "This Entity already have this component");
			return m_Handle.emplace<tnComponent>(args...);
		}

		template <typename tnComponent>
		void DefaultEmplace()
		{
			m_Handle.emplace_or_replace<tnComponent>();
		}

		template<typename tnFlagComponent>
		void Flag()
		{
			static_assert(std::is_base_of_v<FlagComponent, tnFlagComponent>, "This is not a flag component!");
			FE_CORE_ASSERT(!AnyOf<tnFlagComponent>(), "This Entity already have this flag component");
			m_Handle.emplace<tnFlagComponent>();
		}

		template<typename tnFlagComponent>
		void UnFlag()
		{
			m_Handle.remove<tnFlagComponent>();
		}

		//In global storage pool only!
		template<typename tnComponent, typename... Args>
		tnComponent& Replace(Args&&... args)
		{
			FE_CORE_ASSERT(AllOf<Component>(), "This Entity does not have this component yet");
			return m_Handle.replace<Component>(std::forward<Args>(args)...);
		}

		//In global storage pool only!
		template<typename tnComponent, typename... Args>
		tnComponent& EmplaceOrReplace(Args&&... args)
		{
			return m_Handle.emplace_or_replace<tnComponent>(std::forward<Args>(args)...);
		}

		//In global storage pool only!
		template<typename... tnComponents>
		auto& Get()
		{
			FE_CORE_ASSERT(AllOf<tnComponents...>(), "This Entity does not have all of this components");
			return m_Handle.get<tnComponents...>();
		}

		//In default storage pool only!
		template<typename tnComponent, typename... Args>
		tnComponent& GetOrEmplace(Args&&... args)
		{
			return m_Handle.get_or_emplace<tnComponent>(std::forward<Args>(args)...);
		}

		//In default storage pool only!
		//returns pointer(s)
		template<typename... tnComponents>
		auto GetIfExist()
		{
			static_assert(!(std::is_base_of_v<CEntityNode, tnComponents> || ...), "Hierarchy modyfications not yet implemented");

			return m_Handle.try_get<tnComponents...>();
		}

		World* GetWorld() const { return m_World; }

		template<typename tnComponent>
		DataComponent* GetAsDataComponentIfExist()
		{
			return (DataComponent*)m_Handle.try_get<tnComponent>();
		}

	protected:
		friend class EditorLayer;
		friend class EntitiesHierarchy;
		friend class Inspector;
		friend class SceneHierarchyPanel;
		friend class Scene;
		friend class World;
		friend class GameplayWorld;
		friend class Entity;

		ECS_handle m_Handle;
		World* m_World = nullptr;
	};

	
}