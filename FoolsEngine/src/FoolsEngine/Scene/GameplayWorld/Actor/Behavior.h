#pragma once

#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "FoolsEngine\Scene\GameplayWorld\CompPtr.h"

namespace fe
{
	class Behavior
	{
	public:
		Behavior() = default;
		virtual ~Behavior() = default;

		virtual void OnInitialize() {};

		virtual void OnUpdate_FrameStart()	{};
		virtual void OnUpdate_PrePhysics()	{};
		virtual void OnUpdate_Physics()		{};
		virtual void OnUpdate_PostPhysics()	{};
		virtual void OnUpdate_FrameEnd()	{};

		virtual void OnShutdown() {};

		virtual void DrawInspectorWidget() {};

		virtual std::string GetBehaviorName() const { return ""; }
		static std::string GetName() { return ""; }

	protected:
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
		
		Entity	m_HeadEntity;
		UUID	m_UUID;
	};


#define FE_BEHAVIOR_SETUP(type, name) \
	virtual std::string GetBehaviorName() const override { return name; } \
	static std::string GetName() { return name; }

}