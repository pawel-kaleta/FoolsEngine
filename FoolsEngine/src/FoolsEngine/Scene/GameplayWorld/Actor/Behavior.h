#pragma once

#include "FoolsEngine\Scene\SimulationStage.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "FoolsEngine\Scene\GameplayWorld\CompPtr.h"

namespace YAML
{
	class Emitter;
	class Node;
}

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

		virtual EntityID DrawInspectorWidget() { return NullEntityID; }

		virtual std::string GetBehaviorName() const { FE_LOG_CORE_ERROR("Unnamed Behavior"); return "Unnamed Behavior"; }
		static std::string GetName() { return "Base Behavior"; }
		bool IsActive() const { return m_Active; }
		virtual void Serialize(YAML::Emitter& emitter) const { FE_LOG_CORE_ERROR("{0} serialization not implemented!", this->GetBehaviorName()); }
		virtual void Deserialize(YAML::Node& data, GameplayWorld* world) { FE_LOG_CORE_ERROR("{0} deserialization not implemented!", this->GetBehaviorName()); }
		UUID GetUUID() const { return m_UUID; }

	protected:
		virtual void OnInitialize() {};
		virtual void OnActivate() {};

#define _BEHAVIOR_ON_UPDATE_DECLARATION(x) virtual void OnUpdate_##x() {};
		FE_FOR_EACH(_BEHAVIOR_ON_UPDATE_DECLARATION, FE_SIMULATION_STAGES);

		virtual void OnDeactivate() {};
		virtual void OnShutdown() {};

		template<SimulationStage::ValueType stage>
		void RegisterForUpdate(uint32_t priority);

		template<SimulationStage::ValueType stage>
		void UnregisterFromUpdate()
		{
			FE_PROFILER_FUNC();

			FE_LOG_CORE_DEBUG("Behavior Update Unregister");

			Actor(m_HeadEntity).RemoveUpdateEnroll<stage>(this);
		}

		static bool DrawEntity(Entity& entity, const std::string& name);
		
		template<typename tnComponent>
		static bool DrawCompPtr(CompPtr<tnComponent>& compPtr, const std::string& name)
		{
			auto entity = compPtr.GetEntity();
			std::string entity_ID_and_name; 
			bool missingComponent = true;
			if (entity)
			{
				entity_ID_and_name = std::to_string(entity.ID()) + " " + entity.Get<CEntityName>().EntityName;
				missingComponent = !entity.AllOf<tnComponent>();
			}
			if (missingComponent)
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, {0.25f,0.25f,0.25f,1.0f});
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FrameBorderSize, 2.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
			bool selected = ImGui::Button(entity_ID_and_name.c_str(), { ImGui::GetContentRegionAvail().x / 2, ImGui::GetTextLineHeightWithSpacing() });
			
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
				{
					IM_ASSERT(payload->DataSize == sizeof(Entity));
					Entity entity = *(const Entity*)payload->Data;
					if (entity)
						if (entity.AllOf<tnComponent>())
							compPtr.Set(entity);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			ImGui::Text(name.c_str());
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			if (missingComponent)
				ImGui::PopStyleColor();

			return selected;
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
	static std::string GetNameStatic() { return name; }

}