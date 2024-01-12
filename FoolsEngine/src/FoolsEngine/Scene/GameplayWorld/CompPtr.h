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
			uint32_t frameNum = Application::Get().GetFrameCount();
			if (m_PointerLifeSpanFrameNumber == frameNum)
			{
				return GetCashed();
			}
			return GetAndCash();
		}
		
		tnComponent* GetAndCash()
		{
			m_Component = m_Entity.GetIfExist<tnComponent>();
			m_PointerLifeSpanFrameNumber = Application::Get().GetFrameCount();

			return m_Component;
		}

		tnComponent* GetCashed() { return m_Component; }

		void Set(Entity entity)
		{
			m_Entity = entity;
			m_PointerLifeSpanFrameNumber = 0;
		}

		Entity GetEntity() const { return m_Entity; }
	private:
		Entity			m_Entity;
		uint32_t		m_PointerLifeSpanFrameNumber = 0;
		tnComponent*	m_Component = nullptr;
	};
}