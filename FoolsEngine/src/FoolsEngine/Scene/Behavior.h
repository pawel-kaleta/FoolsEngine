#pragma once
#include "Scene.h"
#include "Entity.h"
#include "CompPtr.h"

namespace fe
{
	struct CActorData;

	class BehaviorsRegistry
	{
	public:
		struct BehaviorsRegistryItem
		{
			
		};
		std::vector<BehaviorsRegistryItem> Items;

		static BehaviorsRegistry s_Registry;

		void RegisterBehaviors();

		template <typename tnBehavior>
		void RegisterBehavior()
		{
			Items.push_back(
				BehaviorsRegistryItem{
					
				}
			);
		}
	};

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

		virtual void DrawInspectorWidget() const {};

		virtual std::string GetName() const { return ""; }

	protected:
		template<typename tnSimulationStage>
		void RegisterForUpdate()
		{
			FE_PROFILER_FUNC();

			FE_LOG_CORE_DEBUG("LocalSystem Update Register");

			void (Behavior:: * onUpdateFuncPtr)() = nullptr;
			if (std::is_same_v<tnSimulationStage, SimulationStages::FrameStart	>) onUpdateFuncPtr = &Behavior::OnUpdate_FrameStart;
			if (std::is_same_v<tnSimulationStage, SimulationStages::PrePhysics	>) onUpdateFuncPtr = &Behavior::OnUpdate_PrePhysics;
			if (std::is_same_v<tnSimulationStage, SimulationStages::Physics		>) onUpdateFuncPtr = &Behavior::OnUpdate_Physics;
			if (std::is_same_v<tnSimulationStage, SimulationStages::PostPhysics	>) onUpdateFuncPtr = &Behavior::OnUpdate_PostPhysics;
			if (std::is_same_v<tnSimulationStage, SimulationStages::FrameEnd	>) onUpdateFuncPtr = &Behavior::OnUpdate_FrameEnd;

			FE_CORE_ASSERT(onUpdateFuncPtr, "Did not recognise Simulation Stage!");
			
			Actor(m_HeadEntity).EnrollForUpdate<tnSimulationStage>(this, onUpdateFuncPtr);
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
		static void DrawCompPtr(const CompPtr<tnComponent>& compPtr, const std::string& name);
	private:
		friend class Actor;
		
		Entity	m_HeadEntity;
		UUID	m_UUID;
	};

	template<typename tnComponent>
	void Behavior::DrawCompPtr(const CompPtr<tnComponent>& compPtr, const std::string& name)
	{
		//placeholder implementation

		ImGui::BeginDisabled();
		EntityID entityID = compPtr.GetEntity().ID();
		ImGui::DragInt(name.c_str(), (int*)&entityID);
		ImGui::EndDisabled();
	}

#define FE_BEHAVIOR_SETUP(type, name) \
	virtual std::string GetName() const override { return name; }

}