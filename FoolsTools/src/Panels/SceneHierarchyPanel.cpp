#include "SceneHierarchyPanel.h"

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

		bool nodeClicked = false;

		//m_Scene->GetHierarchy().EnforceSafeOrder();
		auto group = m_Scene->GetRegistry().group<CHierarchyNode, CTransform, CTags, CName>();

		uint32_t current = 1;
		auto node = group.get<CHierarchyNode>(group[current]);
		while (node.HierarchyLvl == 1)
		{
			nodeClicked |= DrawSet(group[current++]);
			node = group.get<CHierarchyNode>(group[current]);
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !nodeClicked)
			m_SelectedSetID = NullSetID;


		ImGui::End();
	}

	bool SceneHierarchyPanel::DrawSet(SetID setID)
	{
		auto& [node, name] = m_Scene->GetRegistry().group<CHierarchyNode, CTransform, CTags, CName>().get<CHierarchyNode, CName>(setID);

		ImGuiTreeNodeFlags flags = 0;

		if (node.Children)
			flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		else
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

		bool selected = (m_SelectedSetID == setID);
		flags |= selected ? ImGuiTreeNodeFlags_Selected : 0;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)setID, flags, name.Name.c_str());

		bool nodeClicked = false;
		if (ImGui::IsItemClicked())
		{
			nodeClicked = true;

			if(!ImGui::IsItemToggledOpen())
				m_SelectedSetID = setID;
		}

		if (opened && node.Children)
		{
			auto children = ChildrenList(setID, m_Scene->GetRegistry());

			auto child = children.Begin();
			while (child != children.End())
				nodeClicked |= DrawSet(*child++);

			ImGui::TreePop();
		}

		return nodeClicked;
	}
}