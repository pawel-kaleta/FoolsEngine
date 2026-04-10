#include "FE_pch.h"

#include "FoolsEngine/Renderer/3 - Command/Render.h"

namespace fe::Command
{
	void Render_OpenGL::DrawIndexed(const Resource::RMeshBindings& meshBindings)
	{
		glDrawElements(GL_TRIANGLES, meshBindings.IndexCount, GL_UNSIGNED_INT, (const void *)(UInt)meshBindings.IndexOffset);
	}
}