#pragma once
#include "ECS.h"
#include "Component.h"

#include <queue>

namespace fe
{
	class SceneHierarchy;
	template <typename Component, typename DataStruct>
	class HierarchicalComponentHandle;
	class Scene;

	struct NativeScript
	{
	public:
		NativeScript() = default;
		NativeScript(NativeScript& other) = delete;
		virtual ~NativeScript() = default;

		virtual void OnCreate()  {};
		virtual void OnUpdate()  {};
		virtual void OnDestroy() {};

	protected:
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
			return m_Handle.emplace_or_replace<TComponent>(std::forward<Args>(args)...);
		}

		//In default storage pool only!
		template<typename... Components>
		auto& Get()
		{
			static_assert(!(std::is_base_of_v<CHierarchicalBase, Components> || ...), "This is a hierarchy managable component. Use GetTransformHandle() or GetTagsHandle()");
			static_assert(!(std::is_base_of_v<CHierarchyNode, Components> || ...), "Hierarchy modyfications not yet implemented");
			FE_CORE_ASSERT(AllOf<Components...>(), "This Set does not have all of those components");
			return m_Handle.get<Components...>();
		}

		HierarchicalComponentHandle<CTransform, Transform> GetTransformHandle();
		HierarchicalComponentHandle<CTags, Tags> GetTagsHandle();

		//In default storage pool only!
		template<typename Component, typename... Args>
		Component& GetOrEmplace(Args&&... args)
		{
			static_assert(!std::is_base_of_v<CHierarchicalBase, Component>, "This is a hierarchy managable component. Use GetTransformHandle() or GetTagsHandle()");
			static_assert(!std::is_base_of_v<CHierarchyNode, Component>, "Hierarchy modyfications not yet implemented");
			return m_Handle.get_or_emplace<Component>(std::forward<Args>(args)...);
		}

		//In default storage pool only!
		template<typename... Components>
		auto GetIfExist()
		{
			static_assert(!(std::is_base_of_v<CHierarchicalBase, Components> || ...), "This is a hierarchy managable component. Use GetTransformHandle() or GetTagsHandle()");
			static_assert(!(std::is_base_of_v<CHierarchyNode, Components> || ...), "Hierarchy modyfications not yet implemented");
			return m_Handle.try_get<Components...>();
		}

		//In default storage pool only!
		template<typename... Components>
		void Remove()
		{
			static_assert(!std::is_base_of_v<CHierarchicalBase, Components> && ..., "Cannot remove Hierarchical components!");
			static_assert(!std::is_base_of_v<CHierarchyNode, Components> && ..., "Cannot remove Hierarchical components!");

			FE_CORE_ASSERT(AllOf<Components...>(), "This Set does not have all of this components");
			m_Handle.erase<Components...>();
		}

		//In default storage pool only!
		template<typename... Components>
		auto RemoveIfExist()
		{
			static_assert(!(std::is_base_of_v<CHierarchicalBase, Components> || ...), "Cannot remove Hierarchical components!");
			static_assert(!(std::is_base_of_v<CHierarchyNode, Components> || ...), "Cannot remove Hierarchical components!");

			return m_Handle.remove<Components...>();
		}

		// Destruction is scheduled
		void Destroy();

		//all pools :)
		auto Storages()
		{
			return m_Handle.storage();
		}

		SetID GetSetID()
		{
			return m_Handle.entity();
		}

	private:
		friend struct CNativeScript;
		ECS_handle m_Handle;
		Scene* m_Scene = nullptr;
	};

	struct CNativeScript : ComponentBase
	{
		//call Instantiate<Script>() immediately after;
		CNativeScript() = default;
		CNativeScript(CNativeScript& other) = delete;

		//should be immediately called after construction!
		template<typename Script, typename... Args>
		void Instantiate(SetID setID, Scene* scene, Args&&... args)
		{
			m_Instance = static_cast<NativeScript*>(new Script(std::forward<Args>(args)...));
			m_Instance->m_Handle = ECS_handle(scene->m_Registry, setID);
			m_Instance->m_Scene = scene;
			m_Instance->OnCreate();
		}

		~CNativeScript()
		{
			m_Instance->OnDestroy();
			delete m_Instance;
		}

	private:
		friend class Set;
		friend class Scene;
		NativeScript* m_Instance = nullptr;
	};
}