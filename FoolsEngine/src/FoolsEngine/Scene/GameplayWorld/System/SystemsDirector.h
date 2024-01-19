#pragma once

#include "System.h"
#include "FoolsEngine\Scene\SimulationStages.h"
#include "FoolsEngine\Core\UUID.h"

namespace fe
{
	class SystemsDirector
	{
	public:
		template<typename tnSimulationStage>
		void EnrollForUpdate(System* system, void (System::* onUpdateFuncPtr)(), uint32_t priority)
		{
			FE_LOG_CORE_DEBUG("GameplayWorld: system EnrollForUpdate");

			constexpr int stage = (int)SimulationStages::EnumFromType<tnSimulationStage>();
			m_SystemUpdateEnrolls[stage].push_back(SystemUpdateEnroll{ system, onUpdateFuncPtr, priority });
			SortSystemUpdateEnrolls(stage);
		}

		void UpdateSystems(SimulationStages::Stages stage);

		template <typename tnSystem>
		tnSystem* CreateSystem()
		{
			FE_PROFILER_FUNC();
			FE_LOG_CORE_DEBUG("System creation");
			static_assert(std::is_base_of_v<System, tnSystem>, "This is not a system!");

			tnSystem* system = new tnSystem();
			system->m_SystemsDirector = this;
			//system->OnInitialize();

			{
				std::unique_ptr<System> up(system);
				m_Systems.push_back(std::move(up));
			}
			return system;
		}

		System* CreateSystemFromName(const std::string& systemTypeName);

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
		using SystemUpdateEnrolls = std::array<std::vector<SystemUpdateEnroll>, (int)SimulationStages::Stages::StagesCount>;

		Systems             m_Systems;
		SystemUpdateEnrolls	m_SystemUpdateEnrolls;

		void SortSystemUpdateEnrolls(int stage);
		void RemoveSystem(System* system);

		template <typename tnSystem>
		System* CreateSystemAsBase()
		{
			return CreateSystem<tnSystem>();
		}
	};
}