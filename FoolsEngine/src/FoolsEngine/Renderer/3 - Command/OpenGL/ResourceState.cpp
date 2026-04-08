#include "FE_pch.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/2 - Resource/OpenGL/Utils.h"
#include "FoolsEngine/Renderer/3 - Command/ResourceState.h"

namespace fe::Command::ResourceState
{
	template <> void Clear<GAPIType::OpenGL>()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

}