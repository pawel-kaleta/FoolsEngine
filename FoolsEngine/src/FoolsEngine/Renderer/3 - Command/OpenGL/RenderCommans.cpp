#include "FE_pch.h"

#include "FoolsEngine\Renderer\3 - Command\RenderCommands.h"

namespace fe::Command
{
	namespace RenderCommands::OpenGL
	{
		void DrawIndexed(const Resource::VertexArray_OpenGL& vertexArray)
		{
			glDrawElements(GL_TRIANGLES, vertexArray.IndexCount, GL_UNSIGNED_INT, (const void *)(uint64_t)vertexArray.IndexOffset);
		}
	}
}