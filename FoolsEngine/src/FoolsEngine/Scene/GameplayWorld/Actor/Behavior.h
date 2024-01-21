#pragma once

#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "FoolsEngine\Scene\GameplayWorld\CompPtr.h"

#include <yaml-cpp\yaml.h>

namespace fe
{
	class Behavior
	{
	public:
		Behavior() = default;
		virtual ~Behavior() = default;

		void Activate();
		void Deactivate();

		void Initialize();
		void Shutdown();

		virtual void DrawInspectorWidget() {};

		virtual std::string GetBehaviorName() const { FE_LOG_CORE_ERROR("Unnamed Behavior"); return "Unnamed Behavior"; }
		static std::string GetName() { return "Base Behavior"; }
		bool IsActive() const { return m_Active; }
		virtual void Serialize(YAML::Emitter& emitter) const { FE_LOG_CORE_ERROR("{0} serialization not implemented!", this->GetBehaviorName()); }
		virtual void Deserialize(YAML::Node& data, GameplayWorld* world) { FE_LOG_CORE_ERROR("{0} deserialization not implemented!", this->GetBehaviorName()); }
		UUID GetUUID() const { return m_UUID; }

	protected:
		virtual void OnInitialize() {};
		virtual void OnActivate() {};

		virtual void OnUpdate_FrameStart()	{};
		virtual void OnUpdate_PrePhysics()  { FE_LOG_CORE_ERROR("Base OnUpdate!"); };
		virtual void OnUpdate_Physics()		{};
		virtual void OnUpdate_PostPhysics()	{};
		virtual void OnUpdate_FrameEnd()	{};

		virtual void OnDeactivate() {};
		virtual void OnShutdown() {};

		template<typename tnSimulationStage>
		void RegisterForUpdate(uint32_t priority)
		{
			FE_PROFILER_FUNC();

			FE_LOG_CORE_DEBUG("Behavior Update Register");

			void (Behavior:: * onUpdateFuncPtr)() = nullptr;
			if (std::is_same_v<tnSimulationStage, SimulationStages::FrameStart	>) onUpdateFuncPtr = &Behavior::OnUpdate_FrameStart;
			if (std::is_same_v<tnSimulationStage, SimulationStages::PrePhysics	>) onUpdateFuncPtr = &Behavior::OnUpdate_PrePhysics;
			if (std::is_same_v<tnSimulationStage, SimulationStages::Physics		>) onUpdateFuncPtr = &Behavior::OnUpdate_Physics;
			if (std::is_same_v<tnSimulationStage, SimulationStages::PostPhysics	>) onUpdateFuncPtr = &Behavior::OnUpdate_PostPhysics;
			if (std::is_same_v<tnSimulationStage, SimulationStages::FrameEnd	>) onUpdateFuncPtr = &Behavior::OnUpdate_FrameEnd;

			FE_CORE_ASSERT(onUpdateFuncPtr, "Did not recognise Simulation Stage!");
			
			Actor(m_HeadEntity).EnrollForUpdate<tnSimulationStage>(this, onUpdateFuncPtr, priority);
		}

		template<typename tnSimulationStage>
		void UnregisterFromUpdate()
		{
			FE_PROFILER_FUNC();

			FE_LOG_CORE_DEBUG("Behavior Update Unregister");

			Actor(m_HeadEntity).RemoveUpdateEnroll<tnSimulationStage>(this);
		}

		static void DrawEntity(Entity entity, const std::string& name)
		{
			//placeholder implementation

			ImGui::BeginDisabled();
			EntityID entityID = entity.ID();
			ImGui::DragInt(name.c_str(), (int*)&entityID);
			ImGui::EndDisabled();
		}
		
		template<typename tnComponent>
		static void DrawCompPtr(const CompPtr<tnComponent>& compPtr, const std::string& name)
		{
			//placeholder implementation

			ImGui::BeginDisabled();
			EntityID entityID = compPtr.GetEntity().ID();
			ImGui::DragInt(name.c_str(), (int*)&entityID);
			ImGui::EndDisabled();
		}
	private:
		friend class Actor;
		friend class SceneSerializerYAML;
		
		Entity	m_HeadEntity;
		UUID	m_UUID;
		bool    m_Active = false;
	};


#define FE_BEHAVIOR_SETUP(type, name) \
	virtual std::string GetBehaviorName() const override { return name; } \
	static std::string GetName() { return name; }

}