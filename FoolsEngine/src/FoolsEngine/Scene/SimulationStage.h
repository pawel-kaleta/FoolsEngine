#pragma once

#include "FoolsEngine\Utils\DeclareEnum.h"

namespace fe
{
	FE_DECLARE_ENUM(SimulationStage,
		FrameStart,
		PrePhysics,
		Physics, 
		PostPhysics,
		FrameEnd,
		Count
	);
}