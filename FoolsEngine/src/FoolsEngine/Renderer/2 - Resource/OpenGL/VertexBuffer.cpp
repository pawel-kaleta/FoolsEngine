#include "FE_pch.h"

#include "FoolsEngine\Renderer\2 - Resource\VertexBuffer.h"

namespace fe::Resource
{
	using namespace Description;

	void VertexBuffer_OpenGL::Create()
	{
		FE_CORE_ASSERT(Usage == Buffer::Usage::Draw, "Vertex Buffer is only for Drawing.");
		
		glCreateBuffers(1, &VertexBufferID);
	}

	void VertexBuffer_OpenGL::Delete()
	{
		glDeleteBuffers(1, &VertexBufferID);
	}
}