#pragma once

namespace fe
{
	namespace SimulationStages
	{
		constexpr char* Names[] = {"FrameStart", "PrePhysics", "Physics", "PostPhysics", "FrameEnd"};
		
		enum class Stages
		{
			FrameStart,
			PrePhysics,
			Physics, 
			PostPhysics,
			FrameEnd,
			StagesCount
		};

		struct FrameStart	{};
		struct PrePhysics	{};
		struct Physics		{};
		struct PostPhysics	{};
		struct FrameEnd		{};

		template <typename SimulationStage>
		inline constexpr Stages EnumFromType()
		{
			if (std::is_same_v<SimulationStage, FrameStart	>) return Stages::FrameStart;
			if (std::is_same_v<SimulationStage, PrePhysics	>) return Stages::PrePhysics;
			if (std::is_same_v<SimulationStage, Physics		>) return Stages::Physics;
			if (std::is_same_v<SimulationStage, PostPhysics	>) return Stages::PostPhysics;
			if (std::is_same_v<SimulationStage, FrameEnd	>) return Stages::FrameEnd;

			static_assert(
				std::is_same_v<SimulationStage, FrameStart	> ||
				std::is_same_v<SimulationStage, PrePhysics	> ||
				std::is_same_v<SimulationStage, Physics		> ||
				std::is_same_v<SimulationStage, PostPhysics	> ||
				std::is_same_v<SimulationStage, FrameEnd	>,
				"Did not recognise Simulation Stage!");

			return Stages::StagesCount;
		}
	};


}