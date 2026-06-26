#include "FE_pch.h"

#include "FoolsEngine/Renderer/2 - Resource/RBuffer.h"
#include "FoolsEngine/Renderer/2 - Resource/OpenGL/Utils.h"

namespace fe::Resource
{
	using namespace Description;

	void RBuffer_OpenGL::Create()
	{
		glCreateBuffers(1, &OpenGLID);

		glNamedBufferStorage(OpenGLID, Size, nullptr, 0);
	}

	void RBuffer_OpenGL::Delete()
	{
		glDeleteBuffers(1, &OpenGLID);
	}
}