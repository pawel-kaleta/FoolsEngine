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

		//m_Scene->GetHierarchy().EnforceSafeOrder();
		auto group = m_Scene->GetRegistry().group<CHierarchyNode, CTransform, CTags, CName>();

		uint32_t current = 1;
		auto node = group.get<CHierarchyNode>(group[current]);
		while (node.HierarchyLvl == 1)
		{
			DrawSet(group[current++]);
			node = group.get<CHierarchyNode>(group[current]);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawSet(SetID setID)
	{
		auto& [node, name] = m_Scene->GetRegistry().group<CHierarchyNode, CTransform, CTags, CName>().get<CHierarchyNode, CName>(setID);

		ImGuiTreeNodeFlags flags = 0;

		if (node.Children)
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		else
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

		bool selected = (m_SelectedSetID == setID);
		flags |= selected ? ImGuiTreeNodeFlags_Selected : 0;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)setID, flags, name.Name.c_str());

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			m_SelectedSetID = setID;

		if (opened && node.Children)
		{
			auto children = ChildrenList(setID, m_Scene->GetRegistry());

			auto child = children.Begin();
			while (child != children.End())
				DrawSet(*child++);

			ImGui::TreePop();
		}
	}
}