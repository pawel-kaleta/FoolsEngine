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
		CActorData() = default;
		CActorData(CActorData& other) :
			m_Behaviors	    (std::move(other.m_Behaviors)),
			m_UpdateEnrolls	(std::move(other.m_UpdateEnrolls))
		{  };
		~CActorData()
		{
			FE_PROFILER_FUNC();
			FE_LOG_CORE_DEBUG("CActorData destruction");
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
	};
}