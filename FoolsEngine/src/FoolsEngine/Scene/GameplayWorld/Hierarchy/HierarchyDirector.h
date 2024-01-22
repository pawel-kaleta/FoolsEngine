#pragma once

#include "FoolsEngine\Scene\ECS.h"
#include "FoolsEngine\Scene\Component.h"

namespace fe
{
	class Scene;
	class Entity;
	class GameplayWorld;

	class HierarchyDirector
	{
	public:
		auto Group() { return m_Registry->group<CEntityNode, CTransformLocal, CTransformGlobal, CTags>(); }
		
		HierarchyDirector(GameplayWorld* world);

		void MakeGlobalTransformsCurrent();

		bool IsOrderSafe()		{ return m_SafeOrder; }
		void EnforceSafeOrder()	{ if (!m_SafeOrder)	RecreateStorageOrder(); }
	private:
		friend class Scene;
		friend class GameplayWorld;
		friend class Entity;		

		GameplayWorld* m_World     = nullptr;
		Registry*      m_Registry  = nullptr;
		bool           m_SafeOrder = true;	

		void CreateNode(EntityID entityID, EntityID parentID);
		void DestroyFlagged();

		// the below is most likely terrible and will be revisited and tested

		// used for sorting flag components acordingly to hierarchy for safe operations (destruction, update, etc.)
		// produces reversed order!
		struct compare {
			const Registry* m_Reg;
			bool operator() (const EntityID left, const EntityID right) {
				const auto& cl = m_Reg->get<CEntityNode>(left);
				const auto& cr = m_Reg->get<CEntityNode>(right);

				return cl.HierarchyLvl < cr.HierarchyLvl;
			}
		} m_Compare{m_Registry};
		
		// sorting ensures parent entities is always after a child in a storage (requirement of overall scene architecture model)
		// additionally improves cache locality when traversing a hierarchy - this part can be amortized
		struct sort {
			using It = std::reverse_iterator<std::vector<EntityID>::iterator>;
			constexpr static int TRESHOLD = 32;
			const Registry* m_Reg;

			//returns last ungrouped
			//basicly QuicSort's partition
			It groupByLvl(It first, It last, uint32_t level)
			{
				FE_PROFILER_FUNC();
				auto& l = first;
				auto& r = last;

				// when there is only 1 level in scope to partition --r in for loop would go out of scope of array, this is a fix for that
				// only first search for *r to swap is guarded, if we prove --r to be safe, we transition to unguarded version
				// ++l is safe because of root (root's HierarchyLvl = 0 while smallest value of level argument is 1)
				{
					while (m_Reg->get<CEntityNode>(*l).HierarchyLvl > level) ++l;
				
					if (l == first) //if l>first, then we found higher HierarchyLvl in scope, r-- is proven safe
						while (m_Reg->get<CEntityNode>(*r).HierarchyLvl <= level && l < r) --r;
					else
						while (m_Reg->get<CEntityNode>(*r).HierarchyLvl <= level) --r;

					if (l < r) 
						std::swap(*l, *r); //if swaping, then we found higher HierarchyLvl in scope, r-- is proven safe
					else
						return l;
				}

				for (;;)
				{
					while (m_Reg->get<CEntityNode>(*l).HierarchyLvl >  level) ++l;
					while (m_Reg->get<CEntityNode>(*r).HierarchyLvl <= level) --r;
				
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
					sortLvl(cut, last);
					last = cut;
					++level;
					length = last - first;
				}
				tailSort(first, last);
			}
			void sortLvl(It first, It last)
			{
				FE_PROFILER_FUNC();

				if (first >= last)
					return;

				auto comp = [&](EntityID l, EntityID r)
				{
					const auto& cl = m_Reg->get<CEntityNode>(l);
					const auto& cr = m_Reg->get<CEntityNode>(r);

					return (cl.Parent > cr.Parent) || ((cl.Parent == cr.Parent) && (l > r));
				};

				std::sort(first, last, comp);
			}

			// the deepest part of the hierarchy is possible to have a long chain of individual nodes
			// groupByLvl() is unoptimal for that
			// TO DO: consider increasing TRESHOLD
			void tailSort(It first, It last)
			{
				FE_PROFILER_FUNC();

				if (first >= last)
					return;

				auto comp = [&](EntityID l, EntityID r)
				{
					const auto& cl = m_Reg->get<CEntityNode>(l);
					const auto& cr = m_Reg->get<CEntityNode>(r);

					return
						(cl.HierarchyLvl > cr.HierarchyLvl) || (
							(cl.HierarchyLvl == cr.HierarchyLvl) && (
								(cl.Parent > cr.Parent) || (
									(cl.Parent == cr.Parent) && (l > r))));
				};

				std::sort(first, last, [&](EntityID l, EntityID r) {
					const auto& cl = m_Reg->get<CEntityNode>(l);
					const auto& cr = m_Reg->get<CEntityNode>(r);

					if (cl.HierarchyLvl == cr.HierarchyLvl)
						if (cl.Parent == cr.Parent)
							return l > r;
						else
							return cl.Parent > cr.Parent;
					else
						return cl.HierarchyLvl > cr.HierarchyLvl;
				});
			}
			
			// underlying entt library expects this operator to perform sorting on a sigle array
			// actual storages are then synced to this reference array in linear time with in-place swaps (only the necessary ones)
			void operator() (It begin, It end, compare Comp) {
				if (end - begin < 3)
					return;
				auto last = end - 1; // end is past last, need last
				auto first_cut = groupByLvl(begin, last, 1); // there are no siblings on lvl 0 (root)
				sortMainBody(begin, first_cut, 2);
			}
		} m_Sort{m_Registry};

		void RecreateStorageOrder();
	};
	
}