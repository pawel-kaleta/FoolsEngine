#pragma once

#include "FoolsEngine/Renderer/3 - Command/ProgramState.h"

namespace fe
{
	struct ProgramManager
	{

	};

	struct ProgramManger_OpenGL final : public ProgramManager
	{
		Command::ProgramState_OpenGL Commands;
	};
}