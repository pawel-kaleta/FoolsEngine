#pragma once

#include "FoolsEngine\Scene\ECS.h"
#include "FoolsEngine\Scene\Component.h"
#include "FoolsEngine\Scene\SimulationStages.h"

#include "Behavior.h"

namespace fe
{
	struct CActorData : ProtectedComponent
	{
	public:
		CActorData() = delete;
		CActorData(EntityID entityID)
			: m_ID(entityID)
		{
			FE_LOG_CORE_DEBUG("CActorData construction");
		};
		CActorData& operator=(CActorData&& other)
		{
			m_Behaviors = std::move(other.m_Behaviors);
			m_UpdateEnrolls = std::move(other.m_UpdateEnrolls);
			m_ID = other.m_ID;
			other.m_ID = NullEntityID;
			return *this;
		};
		CActorData(CActorData&& other)
		{
			m_Behaviors = std::move(other.m_Behaviors);
			m_UpdateEnrolls = std::move(other.m_UpdateEnrolls);
			m_ID = other.m_ID;
			other.m_ID = NullEntityID;
		};
		CActorData& operator=(CActorData& other) = delete;
		CActorData(CActorData& other) = delete;
		~CActorData()
		{
			if (m_ID != NullEntityID)
			{
				FE_PROFILER_FUNC();
				FE_LOG_CORE_DEBUG("CActorData destruction: {0}", m_ID);

				for (size_t i = 0; i < SimulationStages::Count; i++)
				{
					m_UpdateEnrolls[i].clear();
				}

				m_Behaviors.clear();
			}
		};

		FE_COMPONENT_SETUP(CActorData, "ActorData");
	
	private:
		friend class Actor;
		friend class ActorInspector;
		friend class SceneSerializerYAML;
		
		using Behaviors = std::vector<std::unique_ptr<Behavior>>;
		
		struct UpdateEnroll
		{
			Behavior* Behavior;
			void (Behavior::* OnUpdateFuncPtr)();
			uint32_t Priority;
		};
		using UpdateEnrolls = std::array<std::vector<UpdateEnroll>, SimulationStages::Count>;

		Behaviors		m_Behaviors;
		UpdateEnrolls	m_UpdateEnrolls;
		EntityID		m_ID;
	};
}