#pragma once

#include "Entity.h"

#include "FoolsEngine/Foundation/Memory/StringBuilder.h"

#include "FoolsEngine/Application/Application.h"

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
			uint32_t frameNum = Application::Get().m_FrameCount;
			if (m_PointerLifeSpanFrameNumber == frameNum)
			{
				return GetCashed();
			}
			return GetAndCash();
		}
		
		tnComponent* GetAndCash()
		{
			m_Component = m_Entity.GetIfExist<tnComponent>();
			m_PointerLifeSpanFrameNumber = Application::Get().m_FrameCount;

			return m_Component;
		}

		tnComponent* GetCashed() const { return m_Component; }

		void Set(Entity entity)
		{
			if (!entity)
			{
				Context::Log::Core.Warn("Not a valid entity"_FE);
				return;
			}
			if (entity.AllOf<tnComponent>())
			{
				m_Entity = entity;
				m_PointerLifeSpanFrameNumber = 0;
			}
			else
			{
				Context::Log::Core.Error("This entity ({0}) does not have this component ({1})"_FE, entity.Get<CEntityName>().EntityName, tnComponent::GetNameStatic());
			}
		}

		Entity GetEntity() const { return m_Entity; }
	private:
		Entity			m_Entity;
		uint32_t		m_PointerLifeSpanFrameNumber = 0;
		tnComponent*	m_Component = nullptr;
	};
}