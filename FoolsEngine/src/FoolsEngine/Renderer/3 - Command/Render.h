#pragma once

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/2 - Resource/RTexture.h"
#include "FoolsEngine/Renderer/2 - Resource/RMeshBindings.h"

namespace fe::Command
{
	struct Render
	{
		virtual void DrawIndexed(const Resource::RMeshBindings& meshBindings) = 0;
	};

	struct Render_OpenGL final : public Render
	{

		virtual void DrawIndexed(const Resource::RMeshBindings& meshBindings) final override;
	};
}