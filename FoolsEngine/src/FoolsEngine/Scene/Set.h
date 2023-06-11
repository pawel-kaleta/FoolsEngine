#pragma once
#include "FoolsEngine\Debug\Asserts.h"
#include "Scene.h"
#include <entt.hpp>

namespace fe
{
	class Set
	{
	public:
		Set(SetID setID, Scene* scene)
			: m_Scene(scene), m_Handle(ECS_handle(scene->m_Registry, setID)) { }	
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
		inline Component& Emplace(Args&&... args)
		{
			FE_CORE_ASSERT(!AnyOf<Component>(), "This Set already have this component");
			return m_Handle.emplace<Component>(args...);
		}

		//In default storage pool only!
		template<typename Component, typename... Args>
		Component& Replace(Args&&... args)
		{
			FE_CORE_ASSERT(AllOf<Component>(), "This Set does not have this component yet");
			return m_Handle.replace<Component>(std::forward<Args>(args)...);
		}

		//In default storage pool only!
		template<typename Component, typename... Args>
		Component& EmplaceOrReplace(Args&&... args)
		{
			return m_Handle.emplace_or_replace<Component>(std::forward<Args>(args)...);
		}

		//In default storage pool only!
		template<typename... Components>
		auto& Get()
		{
			FE_CORE_ASSERT(AllOf<Components...>(), "This Set does not have all of this components");
			return m_Handle.get<Components...>();
		}

		//In default storage pool only!
		template<typename Component, typename... Args>
		auto& GetOrEmplace(Args&&... args)
		{
			return m_Handle.get_or_emplace<Component>(std::forward<Args>(args)...);
		}

		//In default storage pool only!
		template<typename... Components>
		auto& GetIfExist()
		{
			return m_Handle.try_get<Components...>();
		}

		//In default storage pool only!
		template<typename... Components>
		void Remove()
		{
			FE_CORE_ASSERT(AllOf<Components...>(), "This Set does not have all of this components");
			m_Handle.erase<Components...>();
		}

		//In default storage pool only!
		template<typename... Components>
		auto& RemoveIfExist()
		{
			return m_Handle.remove<Components...>();
		}

		// all pools :)
		void Destroy()
		{
			m_Handle.destroy();
		}

		// all pools :)
		bool IsEmpty()
		{
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

	private:
		friend class Scene;
		ECS_handle m_Handle;
		Scene* m_Scene = nullptr;
	};
}