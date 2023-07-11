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
			}
			MarkDescendantsDirty();
		}

		const DataStruct GetGlobal()
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
	public:
		ChildrenList(ChildrenList&) = default;
		ChildrenList(SetID parentID, Registry& registry)
		{
			auto& parentNode = registry.get<CHierarchyNode>(parentID);
			m_Children = std::vector<SetID>(parentNode.Children);

			SetID currentChild = parentNode.FirstChild;
			for (auto it = m_Children.begin(); it < m_Children.end(); ++it)
			{
				*it = currentChild;
				auto& node = registry.get<CHierarchyNode>(currentChild);
				currentChild = node.NextSibling;
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

		bool IsOrderSafe() { return m_SafeOrder; }
		void EnforceSafeOrder()	{ if (!m_SafeOrder)	SortStep(); }
	private:
		friend class Scene;
		template <typename Component, typename DataStruct>
		friend class HierarchicalComponentHandle;
		Registry& m_Registry;
		bool m_SafeOrder = true;	

		// used for sorting flag components acordingly to hierarchy order for safe operations (destruction, update, etc.)
		// produces reversed order!
		struct compare {
			const Registry& m_Reg;
			bool operator() (const SetID left, const SetID right) {
				const auto& cl = m_Reg.get<CHierarchyNode>(left);
				const auto& cr = m_Reg.get<CHierarchyNode>(right);

				return cl.HierarchyLvl > cr.HierarchyLvl;
			}
		} Compare{m_Registry};
		
		// sorting ensures parent sets is always after a child in a storage (requirement of overall scene architecture model)
		// additionally improves cache locality when traversing a hierarchy - this part is mostly amortized
		struct sort {
			using It = std::reverse_iterator<std::vector<SetID>::iterator>;
			constexpr static int TRESHOLD = 32;
			const Registry& m_Reg;
			static uint32_t m_AStep; // amortization step

			//returns last ungrouped
			It groupByLvl(It first, It last, uint32_t level)
			{
				FE_PROFILER_FUNC();
				auto l = first;
				auto r = last;

				// when there is only 1 level in scope to partition --r in for loop would go out of scope of array, this is a fix for that
				// only first search for *r to swap is guarded, if we prove --r to be safe, we transition to unguarded version to impove performance
				// ++l is safe because of root (root.HierarchyLvl = 0 while smallest value of level argument is 1)
				{
					while (m_Reg.get<CHierarchyNode>(*l).HierarchyLvl > level)
						++l;
				
					if (l == first) //if l>first, then we found higher HierarchyLvl in scope, r-- is proven safe
						while (m_Reg.get<CHierarchyNode>(*r).HierarchyLvl <= level && l < r)
							--r;
					else
						while (m_Reg.get<CHierarchyNode>(*r).HierarchyLvl <= level)
							--r;

					if (l < r) 
						std::swap(*l, *r); //if swaping, then we found higher HierarchyLvl in scope, r-- is proven safe
					else
						return l;
				}

				for (;;)
				{
					while (m_Reg.get<CHierarchyNode>(*l).HierarchyLvl > level)
						++l;

					while (m_Reg.get<CHierarchyNode>(*r).HierarchyLvl <= level)
						--r;
				
					if (l < r)
						std::swap(*l, *r);
					else
						break;
				}
				
				return r;
			}
			void sortMainBody(It first, It last, uint32_t level)
			{
				FE_PROFILER_FUNC();
				auto length = last - first;
				while (length > TRESHOLD)
				{
					auto cut = groupByLvl(first, last, level);
					groupByParent(cut, last);
					last = cut;
					++level;
					length = last - first;
				}
				tailSort(first, last);
			}
			void groupByParent(It first, It last)
			{
				FE_PROFILER_FUNC();
				uint32_t length = last - first;
				uint32_t start = m_AStep % length;
				
				It l = first + start;
				It r = l + 1;
				if (r > last)
					r = first;
				
				const auto& cl = m_Reg.get<CHierarchyNode>(*l);
				const auto& cr = m_Reg.get<CHierarchyNode>(*r);

				if (cl.Parent == cr.Parent)
				{
					if (*l < *r) // additionally ordering by ID to get exact order of a linked list of siblings
						std::swap(*l, *r);
				}
				else
				{
					if (cl.Parent < cr.Parent)
						std::swap(*l, *r);
				}
			}

			// the deepest part of the hierarchy is possible to hove a long chain of individual nodes
			// groupByParent() is highly unoptimal for that
			// TO DO: consider increasing TRESHOLD and using amortization for tail sort
			void tailSort(It first, It last)
			{
				FE_PROFILER_FUNC();

				if (first >= last)
					return;
				std::sort(first, last, [&](SetID l, SetID r) {
					const auto& cl = m_Reg.get<CHierarchyNode>(l);
					const auto& cr = m_Reg.get<CHierarchyNode>(r);

					if (cl.HierarchyLvl == cr.HierarchyLvl)
						return cl.Parent > cr.Parent;
					else
						return cl.HierarchyLvl > cr.HierarchyLvl;
				});
			}
			// underlying entt library expects this oerator to perform sorting on a sigle array
			// actual storages are then synced to this reference array in linear time with in-place swaps (only the necessary ones)
			void operator() (It begin, It end, compare Comp) {
				if (end - begin < 3)
					return;
				--end; // need last, not end
				auto first_cut = groupByLvl(begin, end, 1); // there are no siblings on 1st lvl, so no need for
				sortMainBody(begin, first_cut, 2);
				m_AStep += 1;
			}
		} Sort{m_Registry};


		void CreateNode(Set set, SetID parentID, const std::string& name);

		template <typename Component>
		void MakeGlobalsCurrent()
		{
			FE_PROFILER_FUNC();

			auto dirtyCount = m_Registry.storage<CDirtyFlag<CTransform>>().size();
			bool manyUpdates = (m_Registry.size() / 2) < dirtyCount; // "/2" is just arbitrary, would need to measure
			if (manyUpdates)
			{
				FE_PROFILER_SCOPE("manyUpdates");

				EnforceSafeOrder();

				auto& group = m_Registry.group<CHierarchyNode, CTransform, CTags, CName>();
				
				for (auto it = group.rbegin(); it < group.rend(); it++)
				{
					auto [node, component] = group.get<CHierarchyNode, Component>(*it);
					auto& parent = group.get<Component>(node.Parent);
					component.Global = parent.Global + component.Local;
				}

				m_Registry.storage<CDirtyFlag<Component>>().clear();

				return;
			}

			{
				FE_PROFILER_SCOPE("std::sort");
				m_Registry.sort<CDirtyFlag<Component>>(Compare);
			}

			auto& view = m_Registry.view<CDirtyFlag<Component>>();			
			
			for (auto& setID : view)
			{
				auto& component = m_Registry.get<Component>(setID);
				auto& node = m_Registry.get<CHierarchyNode>(setID);
				auto& parentcomponent = m_Registry.get<Component>(node.Parent);
				component.Global = parentcomponent.Global + component.Local;
			}

			m_Registry.storage<CDirtyFlag<Component>>().clear();
		}

		void SortStep();

		void DestroyFlagged();
	};
	
}