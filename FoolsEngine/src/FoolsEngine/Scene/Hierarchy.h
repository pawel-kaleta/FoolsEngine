#pragma once

#include "ECS.h"
#include "Component.h"

#include <queue>

namespace fe
{
	class Scene;
	class SceneHierarchy;
	class Set;

	template <typename Component>
	struct CDirtyFlag : ComponentBase
	{};

	template <typename Component, typename DataStruct>
	class HierarchicalComponentHandle
	{
	public:
		HierarchicalComponentHandle(
			Component& component,
			CHierarchyNode& node,
			SetID ID,
			SceneHierarchy* hierarchy)
			:
			m_Component(component),
			m_Node(node),
			m_SetID(ID),
			m_Hierarchy(hierarchy),
			m_Registry(hierarchy->m_Registry)
		{
			static_assert(std::is_convertible_v<Component*, CHierarchicalBase*>, "HierarchicalComponentHandle can only operate on CHierarchical<> components");

			if (node.Parent == RootID)
			{
				m_ParentRoot = true;
				return;
			}

			FE_CORE_ASSERT(node.Parent != NullSetID, "Node does not have a parent!");

			m_Parent = &m_Registry.get<Component>(node.Parent);
		}

		const DataStruct& Global()
		{
			return GetGlobal();
		}

		operator DataStruct()
		{
			return GetGlobal();
		}

		operator const DataStruct& ()
		{
			return GetGlobal();
		}
		
		const DataStruct& Local() const
		{
			return m_Component.Local;
		}

		void operator=(const Component& other)
		{
			SetGlobal(other.Global);
		}

		void operator=(const DataStruct& other)
		{
			SetGlobal(other);
		}

		void SetGlobal(const Component& other)
		{
			SetGlobal(other.GetGlobal());
		}

		void SetGlobal(const DataStruct& other)
		{
			FE_CORE_ASSERT(typeid(DataStruct) == typeid(m_Component.Local), "This component does not store this data struct");

			if (!IsDirty(m_SetID))
			{
				if (other == m_Component.Global)
					return;
				if (m_Node.Children)
					MarkDescendantsDirty();
			}
			else
				SetClean(m_SetID);

			m_Component.Global = other;

			if (m_ParentRoot)
				m_Component.Local = m_Component.Global;
			else
			{
				Inherit(m_Node.Parent);
				m_Component.Local = m_Component.Global - m_Parent->Global;
			}
		}

		void SetLocal(const DataStruct& other)
		{
			FE_CORE_ASSERT(typeid(DataStruct) == typeid(m_Component.Global), "This component does not store this data struct");

			if (other == m_Component.Local)
				return;

			m_Component.Local = other;
			if (m_ParentRoot)
				m_Component.Global = m_Component.Local;
			else if (!IsDirty(m_SetID))
			{
				SetDirty(m_SetID);
				MarkDescendantsDirty();
			}
		}

		const DataStruct const GetGlobal()
		{
			if (!m_ParentRoot)
				Inherit(m_SetID);
			return m_Component.Global;
		}
	private:

		void MarkDescendantsDirty()
		{
			std::queue<SetID> toMark;
			toMark.push(m_Node.FirstChild);

			SetID current;
			while (!toMark.empty())
			{
				
				current = toMark.front();
				toMark.pop();
				while (current != NullSetID)
				{
					auto& node = m_Registry.get<CHierarchyNode>(current);
					if (!IsDirty(current))
					{
						toMark.push(node.FirstChild);
						SetDirty(current);
					}
					current = node.NextSibling;
				}
			}
		}

		void Inherit(SetID setID)
		{
			if (!IsDirty(setID))
				return;
			
			auto& node = m_Registry.get<CHierarchyNode>(setID);
			Inherit(node.Parent);
			auto& component = m_Registry.get<Component>(setID);
			auto& parentComponent = m_Registry.get<Component>(node.Parent);
			component.Global = parentComponent.Global + component.Local;
			
			SetClean(setID);
		}


		bool IsDirty()
		{
			return IsDirty(m_SetID);
		}
		bool IsDirty(SetID setID) const
		{
			return m_Registry.all_of<CDirtyFlag<Component>>(setID);
		}

		void SetDirty(SetID setID)
		{
			m_Registry.emplace<CDirtyFlag<Component>>(setID);
		}

		void SetClean(SetID setID) const
		{
			m_Registry.erase<CDirtyFlag<Component>>(setID);
		}

		Component& m_Component;
		Component* m_Parent = nullptr;
		bool m_ParentRoot = false;
		CHierarchyNode& m_Node;
		SetID m_SetID;

		Registry& m_Registry;
		SceneHierarchy* m_Hierarchy;
	};

	class ChildrenList
	{
		ChildrenList(ChildrenList&) = default;
		ChildrenList(SetID parentID, Registry::storage_for_type<CHierarchyNode>& nodesStorage)
		{
			auto& parentNode = nodesStorage.get(parentID);
			m_Children = std::vector<SetID>(parentNode.Children);
			SetID currentChild = parentNode.FirstChild;
			for (auto it = m_Children.begin(); it != m_Children.end(); it++)
			{
				*it = currentChild;
				currentChild = nodesStorage.get(currentChild).NextSibling;
			}
		}
		
		std::vector<SetID>::iterator Begin()
		{
			return m_Children.begin();
		}
		std::vector<SetID>::iterator End()
		{
			return m_Children.end();
		}
		uint32_t Count() const
		{
			return m_Children.size();
		}

	private:
		std::vector<SetID> m_Children;
	};

	class SceneHierarchy
	{
	public:
		SceneHierarchy(Registry& registry)
			: m_Registry(registry)
		{
			registry.group<CHierarchyNode, CTransform, CTags, CName>();
		}

		template<typename Component, typename DataStruct>
		HierarchicalComponentHandle<Component, DataStruct> GetComponent(SetID setID)
		{
			static_assert(std::is_base_of_v<CHierarchicalBase, Component>, "This is not a hierarchy managable component");

			CHierarchyNode& node = m_Registry.get<CHierarchyNode>(setID);
			Component& component = m_Registry.get<Component>(setID);

			HierarchicalComponentHandle<Component, DataStruct> componentHandle(component, node, setID, this);

			return componentHandle;
		}

		void MakeGlobalTagsCurrent();

		void MakeGlobalTransformsCurrent();

		void MakeHierarchyCurrent();

		// amortized sorting to optimize cache locality when traversing hierarchy
		void SortStep();

	private:
		friend class Scene;
		template <typename Component, typename DataStruct>
		friend class HierarchicalComponentHandle;
		Registry& m_Registry;
		bool m_SafeOrder = true;	

		struct compare {
			const Registry& reg;
			bool operator() (const SetID left, const SetID right) {
				const auto& cl = reg.get<CHierarchyNode>(left);
				const auto& cr = reg.get<CHierarchyNode>(right);

				return cl.HierarchyLvl < cr.HierarchyLvl;
			}
		} Compare{m_Registry};
		
		// amortized sorting to optimize cache locality when traversing hierarchy
		struct sort {
			using It = std::reverse_iterator<std::vector<SetID>::iterator>;
			constexpr static int TRESHOLD = 128;
			const Registry& reg;
			uint32_t step = 1;
			It partition_pivot(It begin, It end, uint32_t pivot)
			{
				auto l = begin;
				auto r = end;

				for (;;)
				{
					while (pivot >= reg.get<CHierarchyNode>(*(++l)).HierarchyLvl);
					while (pivot < reg.get<CHierarchyNode>(*(--r)).HierarchyLvl);

					if (l < r)
						std::swap(*l, *r);
					else
						break;
				}

				return l;
			}
			void bigSort(It begin, It end, uint32_t pivot)
			{
				while (end - begin > TRESHOLD)
				{
					auto cut = partition_pivot(begin, end, ++pivot);
					smallSort(begin, cut);
					begin = cut;
				}
				tailSort(begin, end);
			}
			void smallSort(It begin, It end)
			{
				uint32_t length = end - begin;
				uint32_t start = length % step;
				
				const It l = begin + start;
				const It r = l + 1;
				
				const auto& cl = reg.get<CHierarchyNode>(*l);
				const auto& cr = reg.get<CHierarchyNode>(*r);

				if (cl.Parent == cr.Parent)
					if (*l > *r)
						std::swap(*l, *r);
				else
					if (cl.Parent > cr.Parent)
						std::swap(*l, *r);
				
			}
			void tailSort(It begin, It end)
			{
				std::sort(begin, --end, [&](SetID l, SetID r) {
					const auto& cl = reg.get<CHierarchyNode>(l);
					const auto& cr = reg.get<CHierarchyNode>(r);

					if (cl.HierarchyLvl == cr.HierarchyLvl)
						return cl.Parent < cr.Parent;
					else
						return cl.HierarchyLvl < cr.HierarchyLvl;
				});
			}
			void operator() (It begin, It end, compare Comp) {
				bigSort(partition_pivot(--begin, end, 1), end, 1);
				++step;
			}
		} Sort{m_Registry};


		void CreateNode(Set set, SetID parentID, const std::string& name);

		void DestroyNode(SetID setID)
		{
			FE_LOG_CORE_ERROR("Not implemented yet");
			m_SafeOrder = false;
		}

		template <typename Component>
		void MakeGlobalsCurrent()
		{
			bool manyUpdates = (m_Registry.size() / 2) < m_Registry.storage<CDirtyFlag<CTransform>>().size();
			if (manyUpdates)
			{
				if (!m_SafeOrder)
					SortStep();
				auto& group = m_Registry.group<CHierarchyNode, CTransform, CTags, CName>();
				for (auto set : group)
				{
					auto [node, component] = group.get<CHierarchyNode, Component>(set); 
					auto& parent = group.get<Component>(node.Parent);
					component.Global = parent.Global + component.Local;
				}
			}

			m_Registry.sort<CDirtyFlag<Component>>(Compare);

			auto& view = m_Registry.view<CDirtyFlag<Component>>();
			
			for(auto& setID : view)
			{
				auto& component = m_Registry.get<Component>(setID);
				auto& node = m_Registry.get<CHierarchyNode>(setID);
				auto& parentcomponent = m_Registry.get<Component>(node.Parent);
				component.Global = parentcomponent.Global + component.Local;
			}

			m_Registry.storage<CDirtyFlag<Component>>().clear();
		}
	};
	
}