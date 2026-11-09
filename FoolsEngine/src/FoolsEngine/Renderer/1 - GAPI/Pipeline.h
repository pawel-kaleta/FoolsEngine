#pragma once

#include "FoolsEngine/Foundation/Containers/Arena.h"

#include "Context.h"
#include "Data.h"
#include "Resource.h"
#include "Descriptors.h"

namespace fe::Render::GAPI
{
	GID		CreateGraphicsPipelineCmd(GID vertexShader, GID fragmentShader, const Descriptors::Raster::Spec& spec, UInt rootDataSize);
	void	DestroyPipelineCmd(GID pipeline);
}