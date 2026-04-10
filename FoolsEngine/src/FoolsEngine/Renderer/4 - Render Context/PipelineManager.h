#pragma once

#include "FoolsEngine/Renderer/3 - Command/PipelineState.h"

namespace fe
{
	struct PipelineManager
	{

	};

	struct PipelineManager_OpenGL final : public PipelineManager
	{
		Command::PipelineState_OpenGL Commands;
	};
}