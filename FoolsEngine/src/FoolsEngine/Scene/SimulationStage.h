#pragma once

#include "FoolsEngine\Utils\DeclareEnum.h"

namespace fe
{
#define FE_SIMULATION_STAGES FrameStart, PrePhysics, Physics, PostPhysics, FrameEnd

	FE_DECLARE_ENUM(SimulationStage,
		FE_SIMULATION_STAGES,
		Count
	);

}