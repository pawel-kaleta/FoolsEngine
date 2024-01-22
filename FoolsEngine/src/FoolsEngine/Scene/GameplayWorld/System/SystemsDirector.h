#pragma once

#include "System.h"
#include "FoolsEngine\Scene\SimulationStages.h"
#include "FoolsEngine\Core\UUID.h"

namespace fe
{
	class SystemsDirector
	{
	public:
		template<SimulationStages::Stages stage>
		void EnrollForUpdate(System* system, void (System::* onUpdateFuncPtr)(), uint32_t priority)
		{
			FE_LOG_CORE_DEBUG("GameplayWorld: system EnrollForUpdate");

			m_SystemUpdateEnrolls[(size_t)stage].push_back(SystemUpdateEnroll{ system, onUpdateFuncPtr, priority });
			SortSystemUpdateEnrolls(stage);
		}

		template<SimulationStages::Stages stage>
		void RemoveUpdateEnroll(System* system)
		{
			auto& enrolls = m_SystemUpdateEnrolls[(size_t)stage];

			int found = false;
			int enrollPos;
			for (int j = 0; j < enrolls.size(); ++j)
			{
				if (enrolls[j].System == system)
				{
					found = true;
					enrollPos = j;
					break;
				}
			}

			if (found)
			{
				for (size_t last = enrolls.size() - 1; enrollPos < last; ++enrollPos)
				{
					std::swap(enrolls[enrollPos], enrolls[enrollPos + 1]);
				}

				enrolls.pop_back();
			}
		}

		void UpdateSystems(SimulationStages::Stages stage);

		template <typename tnSystem>
		tnSystem* CreateSystem()
		{
			FE_PROFILER_FUNC();
			FE_LOG_CORE_DEBUG("System creation");
			static_assert(std::is_base_of_v<System, tnSystem>, "This is not a system!");

			tnSystem* system = new tnSystem();
			m_Systems.emplace_back((System*)system);
			
			system->m_SystemsDirector = this;
			system->OnInitialize();

			return system;
		}

		System* GetSystemFromName(const std::string& name);

		System* GetSystemFromUUID(UUID uuid) const
		{
			for (auto& system : m_Systems)
			{
				if (uuid == system->m_UUID)
				{
					return system.get();
				}
			}
			return nullptr;
		}
	private:
		friend class SystemsInspector;
		friend class SystemsRegistry;
		friend class SceneSerializerYAML;

		using Systems = std::vector<std::unique_ptr<System>>;

		struct SystemUpdateEnroll
		{
			System* System;
			void (System::* OnUpdateFuncPtr)();
			uint32_t Priority;
		};
		using SystemUpdateEnrolls = std::array<std::vector<SystemUpdateEnroll>, SimulationStages::Count>;

		Systems             m_Systems;
		SystemUpdateEnrolls	m_SystemUpdateEnrolls;

		void SortSystemUpdateEnrolls(SimulationStages::Stages stage);
		void RemoveSystem(System* system);

		System* CreateSystemFromName(const std::string& systemTypeName);
		
		template <typename tnSystem>
		System* CreateSystemAsBase() { return CreateSystem<tnSystem>(); }
	};
}