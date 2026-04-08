#include "FE_pch.h"

#include "FoolsEngine/Renderer/3 - Command/Render.h"

namespace fe::Command
{
	namespace Render
	{
		template <> void DrawIndexed<GAPIType::OpenGL>(const Resource::RMeshBindings<GAPIType::OpenGL>& meshBindings)
		{
			glDrawElements(GL_TRIANGLES, meshBindings.IndexCount, GL_UNSIGNED_INT, (const void *)(UInt)meshBindings.IndexOffset);
		}
	}
}