#pragma once

#include "FoolsEngine\Core\Application.h"
#include "Entity.h"

namespace fe
{
	template <typename tnComponent>
	class CompPtr
	{
	public:
		CompPtr() = default;
		CompPtr(Entity entity) : m_Entity(entity) { };

		tnComponent* Get()
		{
			uint32_t frameNum = Application::GetFrameCount();
			if (m_PointerLifeSpanFrameNumber == frameNum)
			{
				return GetCashed();
			}
			return GetAndCash();
		}
		
		tnComponent* GetAndCash()
		{
			m_Component = m_Entity.GetIfExist<tnComponent>();
			m_PointerLifeSpanFrameNumber = Application::GetFrameCount();

			return m_Component;
		}

		tnComponent* GetCashed() { return m_Component; }

		void Set(Entity entity)
		{
			if (!entity)
			{
				FE_LOG_CORE_WARN("Not a valid entity");
				return;
			}
			if (entity.AllOf<tnComponent>())
			{
				m_Entity = entity;
				m_PointerLifeSpanFrameNumber = 0;
			}
			else
			{
				FE_LOG_CORE_ERROR("This entity ({0}) does not have this component ({1})", entity.Get<CEntityName>().EntityName, tnComponent::GetNameStatic());;
			}
		}

		Entity GetEntity() const { return m_Entity; }
	private:
		Entity			m_Entity;
		uint32_t		m_PointerLifeSpanFrameNumber = 0;
		tnComponent*	m_Component = nullptr;
	};
}