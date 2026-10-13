#pragma once

#include "Behavior.h"

#include "FoolsEngine/Scene/ECS.h"
#include "FoolsEngine/Scene/Component.h"
#include "FoolsEngine/Scene/SimulationStage.h"

#include "FoolsEngine/Foundation/Memory/Arena.h"
#include "FoolsEngine/Foundation/Utils/TypeSetup.h"

namespace fe
{
	struct CActorData : ProtectedComponent
	{
	public:
		CActorData() = delete;
		CActorData(EntityID entityID)
			: m_ID(entityID)
		{ };
		CActorData& operator=(CActorData&& other) noexcept = default;
		//{
		//	m_Behaviors = std::move(other.m_Behaviors);
		//	m_UpdateEnrolls = std::move(other.m_UpdateEnrolls);
		//	m_ID = other.m_ID;
		//	other.m_ID = NullEntityID;
		//	return *this;
		//};
		CActorData(CActorData&& other) noexcept = default;
		//{
		//	m_Behaviors = std::move(other.m_Behaviors);
		//	m_UpdateEnrolls = std::move(other.m_UpdateEnrolls);
		//	m_ID = other.m_ID;
		//	other.m_ID = NullEntityID;
		//};
		CActorData& operator=(CActorData& other) = delete;
		CActorData(CActorData& other) = delete;
		~CActorData()
		{
			if (m_ID != NullEntityID)
			{
				FE_PROFILER_FUNC();
				FE_LOG_CORE_DEBUG("CActorData destruction: {0}", m_ID);

				for (size_t i = 0; i < SimulationStage::Count; i++)
				{
					m_UpdateEnrolls[i].Release();
				}

				m_Behaviors.Release();
			}
		};

		FE_COMPONENT_SETUP(CActorData, "ActorData");
	
	private:
		friend class Actor;
		friend class ActorInspector;
		friend class SceneSerializerYAML;
		
		struct UpdateEnroll
		{
			FE_TYPE_SETUP(UpdateEnroll)

			Behavior* Behavior;
			void (Behavior::* OnUpdateFuncPtr)();
			uint32_t Priority;
		};


		Array<DynamicArena<UpdateEnroll>, SimulationStage::Count> m_UpdateEnrolls;
		DynamicArena<Behavior*> m_Behaviors;
		EntityID m_ID;
	};
}