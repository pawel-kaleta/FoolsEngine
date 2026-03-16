#include "FE_pch.h"

#include "FoolsEngine/Renderer/2 - Resource/StaticBuffer.h"
#include "FoolsEngine/Renderer/2 - Resource/OpenGL/Utils.h"

namespace fe::Resource
{
	using namespace Description;

	void StaticBuffer_OpenGL::Create()
	{
		glCreateBuffers(1, &OpenGLID);
	}

	void StaticBuffer_OpenGL::Delete()
	{
		glDeleteBuffers(1, &OpenGLID);
	}

	void StaticBuffer_OpenGL::Upload(size_t size, const void* data)
	{
		Size = (uint32_t)size;
		glNamedBufferData(OpenGLID, size, data, GL_STATIC_DRAW);
	}

	void StaticBuffer_OpenGL::Update(size_t targetOffset, size_t size, const void* data)
	{
		glNamedBufferSubData(OpenGLID, targetOffset, size, data);
	}
}