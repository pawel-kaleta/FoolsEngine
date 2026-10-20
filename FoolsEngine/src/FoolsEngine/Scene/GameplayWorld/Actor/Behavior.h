#pragma once

#include "FoolsEngine/Scene/SimulationStage.h"
#include "FoolsEngine/Scene/GameplayWorld/Entity.h"
#include "FoolsEngine/Scene/GameplayWorld/CompPtr.h"

#include "FoolsEngine/Foundation/Containers/StringBuilder.h"

#include <string>

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

		virtual String GetBehaviorName() const { return String("Unnamed Behavior"_FE); }
		static String GetName() { return String("Base Behavior"_FE); }

		virtual void Serialize(YAML::Emitter& emitter) const { Context::Log::Core.Error("{0} serialization not implemented!"_FE, this->GetBehaviorName()); }
		virtual void Deserialize(YAML::Node& data, GameplayWorld* world) { Context::Log::Core.Error("{0} deserialization not implemented!"_FE, this->GetBehaviorName()); }


		template<SimulationStage::ValueType stage>
		void RegisterForUpdate(uint32_t priority);

		template<SimulationStage::ValueType stage>
		void UnregisterFromUpdate()
		{
			FE_PROFILER_FUNC();

			Actor(m_HeadEntity).RemoveUpdateEnroll<stage>(this);
		}

	
		Entity	m_HeadEntity;
		UUID	m_UUID;
		bool    m_Active = false;

	protected:
		virtual void OnInitialize() {};
		virtual void OnActivate() {};

#define _BEHAVIOR_ON_UPDATE_DECLARATION(x) virtual void OnUpdate_##x() {};
		FE_FOR_EACH(_BEHAVIOR_ON_UPDATE_DECLARATION, FE_SIMULATION_STAGES);

		virtual void OnDeactivate() {};
		virtual void OnShutdown() {};

		static bool DrawEntity(Entity& entity, String name);
		
		template<typename tnComponent>
		static bool DrawCompPtr(CompPtr<tnComponent>& compPtr, String name)
		{
			Pile p;

			StringBuilderSA<Pile> entity_ID_and_name;
			entity_ID_and_name.Init(&p);

			CString name_c = GetCString(name, &p);

			Entity entity = compPtr.GetEntity();
			bool missingComponent = true;
			if (entity)
			{
				entity_ID_and_name.Append(std::to_string(entity.ID()).c_str());
				entity_ID_and_name.Append(" ", 2);
				entity_ID_and_name.Append(entity.Get<CEntityName>().EntityName);

				missingComponent = !entity.AllOf<tnComponent>();
			}
			if (missingComponent)
				ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, {0.25f,0.25f,0.25f,1.0f});
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FrameBorderSize, 2.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
			ImGui::PushID(name_c.Data);
			bool selected = ImGui::Button(entity_ID_and_name.GetCString(&p).Data, {ImGui::GetContentRegionAvail().x / 2, ImGui::GetTextLineHeightWithSpacing()});
			ImGui::PopID();
			
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
			ImGui::Text(name_c.Data);
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			if (missingComponent)
				ImGui::PopStyleColor();

			return selected;
		}
	};


#define FE_BEHAVIOR_SETUP(type, name) \
	virtual String GetBehaviorName() const override { return name; } \
	static String GetNameStatic() { return name; }

}