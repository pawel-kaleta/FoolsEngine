#include "FE_pch.h"

#include "FoolsEngine\Renderer\2 - Resource\IndexBuffer.h"

namespace fe::Resource
{
	using namespace Description;

	void IndexBuffer_OpenGL::Create()
	{
		FE_CORE_ASSERT(Usage == Buffer::Usage::Draw, "Index Buffer is only for Drawing.");
		glCreateBuffers(1, &IndexBufferOpenGLID);
	}

	void IndexBuffer_OpenGL::Destroy()
	{
		FE_PROFILER_FUNC();
		glDeleteBuffers(1, &IndexBufferOpenGLID);
	}
}