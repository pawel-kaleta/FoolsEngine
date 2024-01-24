#include "WorldHierarchyPanel.h"

#include <sstream>

namespace fe
{
	void WorldHierarchyPanel::SetScene(const Ref<Scene>& scene)
	{
		m_Scene = scene;
		m_SelectedEntityID = NullEntityID;;
	}

	void WorldHierarchyPanel::OnImGuiRender()
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

	bool WorldHierarchyPanel::DrawEntity(EntityID entityID)
	{
		FE_PROFILER_FUNC();

		auto allGroup = m_Scene->GetGameplayWorld()->GetHierarchy().Group();
		auto& node = allGroup.get<CEntityNode>(entityID);

		Entity entity(entityID, m_Scene->GetGameplayWorld());

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

		if (node.ChildrenCount)
			flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		else
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

		bool selected = (m_SelectedEntityID == entityID);
		flags |= selected ? ImGuiTreeNodeFlags_Selected : 0;

		std::string name = entity.GetNameSignature();
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entityID, flags, name.c_str());

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			// Set payload to carry the index of our item (could be anything)
			ImGui::SetDragDropPayload("Entity", &entity, sizeof(entity));

			// Display preview (could be anything, e.g. when dragging an image we could decide to display
			// the filename and a small preview of the image, etc.)
			ImGui::Text(name.c_str());

			ImGui::EndDragDropSource();
		}

		bool nodeClicked = false;
		if (!ImGui::IsItemToggledOpen() && ImGui::IsItemClicked())
		{
			m_SelectedEntityID = entityID;
			nodeClicked = true;
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
		{
			nodeClicked = true;	

			ImGui::OpenPopup(name.c_str());
		}

		bool create_child_owned_entity	= false;
		bool create_child_actor			= false;
		if (ImGui::BeginPopup(name.c_str()))
		{
			// creating children before drawing all existing children is unsafe
			create_child_owned_entity = ImGui::MenuItem("Create Entity");
			create_child_actor		  = ImGui::MenuItem("Create Actor" );

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