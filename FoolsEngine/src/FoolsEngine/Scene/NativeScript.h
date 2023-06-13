#pragma once
#include "ECS.h"
#include "Set.h"
#include "Component.h"

namespace fe
{
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

		// Destruction should be scaduled, as it would delete also object that we are inside of now
		//
		//all pools :)
		//void Destroy()
		//{
		//	m_Handle.destroy(); 
		//}

		//all pools :)
		auto& Storages()
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

	struct CNativeScript
	{
		//call Instantiate<Script>() immediately after;
		CNativeScript() = default;
		CNativeScript(CNativeScript& other) = delete;

		//should be immediately called after construction!
		template<typename Script, typename... Args>
		Script& Instantiate(SetID setID, Scene* scene, Args&&... args)
		{
			m_Instance = static_cast<NativeScript*>(new Script(std::forward<Args>(args)...));
			m_Instance->m_Handle = ECS_handle(scene->m_Registry, setID);
			m_Instance->m_Scene = scene;
			return (Script&)m_Instance;
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