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

		m_Scene->GetHierarchy().EnforceSafeOrder();
		auto group = m_Scene->GetRegistry().group<CHierarchyNode, CTransform, CTags, CName>();

		for (auto current = ++group.rbegin(); current != group.rend(); ++current) { //first set is root
			if (group.get<CHierarchyNode>(*current).HierarchyLvl > 1) // other levels drawn recursively
				break;

			nodeClicked |= DrawSet(*current);
		}

		if (ImGui::IsWindowHovered() && !nodeClicked)
		{
			if (ImGui::IsMouseClicked(0))
				m_SelectedSetID = NullSetID;

			if (ImGui::IsMouseClicked(1))
				ImGui::OpenPopup("Context menu popup");
		}

		if (ImGui::BeginPopup("Context menu popup"))
		{
			if (ImGui::MenuItem("Create Empty Set"))
				m_Scene->CreateSet();

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	bool SceneHierarchyPanel::DrawSet(SetID setID)
	{
		FE_PROFILER_FUNC();

		auto& [node, name] = m_Scene->GetRegistry().group<CHierarchyNode, CTransform, CTags, CName>().get<CHierarchyNode, CName>(setID);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

		if (node.Children)
			flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		else
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

		bool selected = (m_SelectedSetID == setID);
		flags |= selected ? ImGuiTreeNodeFlags_Selected : 0;

		std::string set_ID_and_name = std::to_string(setID) + " " + name.Name;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)setID, flags, set_ID_and_name.c_str());

		bool nodeClicked = false;

		if (!ImGui::IsItemToggledOpen() && ImGui::IsItemClicked())
		{
			m_SelectedSetID = setID;
			nodeClicked = true;
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
		{
			nodeClicked = true;	

			ImGui::OpenPopup(set_ID_and_name.c_str());
		}

		bool create_child = false;
		if (ImGui::BeginPopup(set_ID_and_name.c_str()))
		{
			// creating child before drawing all children is unsafe
			create_child = ImGui::MenuItem("Create Empty child Set");
			if (ImGui::MenuItem("Destroy Set"))
				Set(setID, &*m_Scene).Destroy();

			ImGui::EndPopup();
		}

		if (opened && node.Children)
		{
			auto children = ChildrenList(setID, m_Scene->GetRegistry());

			auto child = children.Begin();
			while (child != children.End())
				nodeClicked |= DrawSet(*child++);

			ImGui::TreePop();
		}

		if (create_child)
			m_Scene->CreateSet(setID);

		return nodeClicked;
	}
}