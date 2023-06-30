#pragma once
#include "FoolsEngine\Debug\Asserts.h"
#include "ECS.h"
#include "Component.h"

namespace fe
{
	class NativeScript;
	class CNativeScript;
	class Scene;
	template <typename Component, typename DataStruct>
	class HierarchicalComponentHandle;
	class SceneHierarchy;

	// Handler to a collection of component instances within a particular scene accessible by a common SetID
	class Set
	{
	public:
		Set(SetID setID, Scene* scene);
		Set(const Set& other) = default;
		Set() = default;

		//In default storage pool only!
		template<typename... Components>
		bool AllOf()
		{
			return m_Handle.all_of<Components...>();
		}

		//In default storage pool only!
		template<typename... Components>
		bool AnyOf()
		{
			return m_Handle.any_of<Components...>();
		}

		//In default storage pool only!
		template<typename Component, typename... Args>
		Component& Emplace(Args&&... args)
		{
			static_assert(!std::is_base_of_v<CHierarchicalBase, Component> && !std::is_base_of_v<CHierarchyNode, Component>, "Cannot emplace Hierarchical components!");
			
			FE_CORE_ASSERT(!AnyOf<Component>(), "This Set already have this component");
			SetID setID = m_Handle.entity();
			return m_Handle.emplace<Component>(args...);
		}

		//In default storage pool only!
		template<typename Component, typename... Args>
		Component& Replace(Args&&... args)
		{
			static_assert(!std::is_base_of_v<CHierarchicalBase, Component> && !std::is_base_of_v<CHierarchyNode, Component>, "Cannot replace Hierarchical components!");
			FE_CORE_ASSERT(AllOf<Component>(), "This Set does not have this component yet");
			return m_Handle.replace<Component>(std::forward<Args>(args)...);
		}

		//In default storage pool only!
		template<typename Component, typename... Args>
		Component& EmplaceOrReplace(Args&&... args)
		{
			static_assert(!std::is_base_of_v<CHierarchicalBase, Component> && !std::is_base_of_v<CHierarchyNode, Component>, "Cannot emplace Hierarchical components!");
			return m_Handle.emplace_or_replace<Component>(std::forward<Args>(args)...);
		}

		//In default storage pool only!
		template<typename... Components>
		auto& Get()
		{
			static_assert(!(std::is_base_of_v<CHierarchicalBase, Components> || ...), "This is a hierarchy managable component. Use GetTransformHandle() or GetTagsHandle()");
			static_assert(!(std::is_base_of_v<CHierarchyNode, Components> || ...), "Hierarchy modyfications not yet implemented");
			FE_CORE_ASSERT(AllOf<Components...>(), "This Set does not have all of this components");
			return m_Handle.get<Components...>();
		}

		HierarchicalComponentHandle<CTransform, Transform> GetTransformHandle();
		HierarchicalComponentHandle<CTags, TagsBase> GetTagsHandle();

		//In default storage pool only!
		template<typename Component, typename... Args>
		Component& GetOrEmplace(Args&&... args)
		{
			static_assert(!std::is_base_of_v<CHierarchicalBase, Component> && !std::is_base_of_v<CHierarchyNode, Component>, "Cannot get Hierarchical components! Use GetTransformHandle() or GetTagsHandle()");
			return m_Handle.get_or_emplace<Component>(std::forward<Args>(args)...);
		}

		//In default storage pool only!
		template<typename... Components>
		auto& GetIfExist()
		{
			static_assert(!(std::is_base_of_v<CHierarchicalBase, Components> || ...), "This is a hierarchy managable component. Use GetTransformHandle() or GetTagsHandle()");
			static_assert(!(std::is_base_of_v<CHierarchyNode, Components> || ...), "Hierarchy modyfications not yet implemented");

			return m_Handle.try_get<Components...>();
		}

		//In default storage pool only!
		template<typename... Components>
		void Remove()
		{
			static_assert(!(std::is_base_of_v<CHierarchicalBase, Components> || ...), "Cannot remove Hierarchical components!");
			static_assert(!(std::is_base_of_v<CHierarchyNode, Components> || ...), "Cannot remove Hierarchical components!"); 
			
			FE_CORE_ASSERT(AllOf<Components...>(), "This Set does not have all of this components");
			m_Handle.erase<Components...>();
		}

		//In default storage pool only!
		template<typename... Components>
		auto& RemoveIfExist()
		{
			static_assert(!(std::is_base_of_v<CHierarchicalBase, Components> || ...), "Cannot remove Hierarchical components!");
			static_assert(!(std::is_base_of_v<CHierarchyNode, Components> || ...), "Cannot remove Hierarchical components!");
			
			return m_Handle.remove<Components...>();
		}

		// all pools :)
		void Destroy()
		{
			FE_LOG_CORE_ERROR("Not implemented yet!");
			//m_Handle.destroy();
		}

		// all pools :)
		bool IsEmpty()
		{
			// Hierarchical components may still exist!
			return m_Handle.orphan();
		}

		//all pools :)
		auto& Storages()
		{
			return m_Handle.storage();
		}

		const SetID ID()
		{
			return m_Handle.entity();
		}
		operator SetID() const
		{
			return m_Handle.operator entt::id_type();
		}
		operator bool() const
		{
			return m_Handle.operator bool() && m_Scene;
		}

		//In default storage pool only!
		template<typename Script, typename... Args>
		Script& AddScript(Args&&... args)
		{
			FE_CORE_ASSERT(!AnyOf<CNativeScript>(), "This Set already have script component");
			auto& scriptComponent = m_Handle.emplace<CNativeScript>();
			scriptComponent.Instantiate<Script>(ID(), m_Scene, std::forward<Args>(args)...);
			return (Script&) scriptComponent.m_Instance;
		}

		//In default storage pool only!
		template<typename Script>
		Script& GetScript()
		{
			static_assert(std::is_convertible_v<Script*, NativeScript*>, "This is not a NativeScript!");

			FE_CORE_ASSERT(AnyOf<CNativeScript>(), "This Set does not have script component");
			auto& scriptComponent = m_Handle.get<CNativeScript>();

			//TO DO: could still be a different script, need to verify
			return (Script&)scriptComponent.m_Instance;
		}

		Scene* GetScene() { return m_Scene; }
	private:
		friend class Scene;
		friend class SceneHierarchy;
		ECS_handle m_Handle;
		fe::Scene* m_Scene = nullptr;
	};
}