#include "SceneHierarchyPanel.h"

#include <sstream>

namespace fe
{
	void SceneHierarchyPanel::SetScene(const Ref<Scene>& scene)
	{
		m_Scene = scene;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

		ImGui::Begin("Scene Hierarchy");

		if (m_Scene == nullptr)
		{
			ImGui::End();
			return;
		}

		bool nodeClicked = false;

		m_Scene->GetGameplayWorld()->GetHierarchy().EnforceSafeOrder();
		auto group = m_Scene->GetGameplayWorld()->GetHierarchy().Group();

		for (auto current = ++group.rbegin(); current != group.rend(); ++current) { //first entity is root
			auto& node = group.get<CEntityNode>(*current);
			if (node.HierarchyLvl > 1) // other levels drawn recursively
				break;

			nodeClicked |= DrawEntity(*current);
		}

		if (ImGui::IsWindowHovered() && !nodeClicked)
		{
			if (ImGui::IsMouseClicked(0))
				m_SelectedEntityID = NullEntityID;

			if (ImGui::IsMouseClicked(1))
				ImGui::OpenPopup("Context menu popup");
		}

		if (ImGui::BeginPopup("Context menu popup"))
		{
			if (ImGui::MenuItem("Create Actor"))
				m_Scene->GetGameplayWorld()->CreateActor();

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	bool SceneHierarchyPanel::DrawEntity(EntityID entityID)
	{
		FE_PROFILER_FUNC();

		auto& nameStorage = m_Scene->GetGameplayWorld()->GetRegistry().storage<CEntityName>();
		auto allGroup = m_Scene->GetGameplayWorld()->GetHierarchy().Group();

		auto& node = allGroup.get<CEntityNode>(entityID);
		auto& name = nameStorage.get(entityID);

		Entity entity(entityID, m_Scene->GetGameplayWorld());

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

		if (node.ChildrenCount)
			flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		else
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

		bool selected = (m_SelectedEntityID == entityID);
		flags |= selected ? ImGuiTreeNodeFlags_Selected : 0;

		std::string entity_ID_and_name = std::to_string(entityID);

		if (entity.IsHead())
		{
			entity_ID_and_name += " [" + name.EntityName + "]";
		}
		else
		{
			auto& headComp = entity.Get<CHeadEntity>();

			entity_ID_and_name += " [" + nameStorage.get(headComp.HeadEntity).EntityName + "]";

			entity_ID_and_name += " " + name.EntityName;
		}


		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entityID, flags, entity_ID_and_name.c_str());

		bool nodeClicked = false;

		if (!ImGui::IsItemToggledOpen() && ImGui::IsItemClicked())
		{
			m_SelectedEntityID = entityID;
			nodeClicked = true;
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
		{
			nodeClicked = true;	

			ImGui::OpenPopup(entity_ID_and_name.c_str());
		}

		bool create_child_owned_entity	= false;
		bool create_child_actor			= false;
		if (ImGui::BeginPopup(entity_ID_and_name.c_str()))
		{
			// creating children before drawing all existing children is unsafe
			create_child_owned_entity	= ImGui::MenuItem("Create Entity");
			create_child_actor		= ImGui::MenuItem("Create Actor");

			if (ImGui::MenuItem("Destroy"))
			{
				Entity(entityID, m_Scene->GetGameplayWorld()).Destroy();
				if (m_SelectedEntityID == entityID)
					SetSelection(NullEntityID);
			}
			ImGui::EndPopup();
		}

		if (opened && node.ChildrenCount)
		{
			auto children = ChildrenList(entityID, & m_Scene->GetGameplayWorld()->GetRegistry());

			auto child = children.Begin();
			while (child != children.End())
				nodeClicked |= DrawEntity(*child++);

			ImGui::TreePop();
		}

		if (create_child_owned_entity)
			entity.CreateChildEntity();
		if (create_child_actor)
			entity.CreateAttachedActor();

		return nodeClicked;
	}
}