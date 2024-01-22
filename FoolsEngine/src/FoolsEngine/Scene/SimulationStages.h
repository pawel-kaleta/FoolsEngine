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

		inline constexpr size_t Count = (size_t)Stages::StagesCount;


		//struct FrameStart	{};
		//struct PrePhysics	{};
		//struct Physics		{};
		//struct PostPhysics	{};
		//struct FrameEnd		{};
	};
}