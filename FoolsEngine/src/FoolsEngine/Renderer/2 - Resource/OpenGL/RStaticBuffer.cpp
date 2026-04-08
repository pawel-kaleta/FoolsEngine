#include "FE_pch.h"

#include "FoolsEngine/Renderer/2 - Resource/RStaticBuffer.h"
#include "FoolsEngine/Renderer/2 - Resource/OpenGL/Utils.h"

namespace fe::Resource
{
	using namespace Description;

	void RStaticBuffer<GAPIType::OpenGL>::Create()
	{
		glCreateBuffers(1, &OpenGLID);
		glNamedBufferData(OpenGLID, Size, nullptr, GL_STATIC_DRAW);
	}

	void RStaticBuffer<GAPIType::OpenGL>::Delete()
	{
		glDeleteBuffers(1, &OpenGLID);
	}

	void RStaticBuffer<GAPIType::OpenGL>::Replace(Splice<Byte> data)
	{
		Size = data.Count;
		glNamedBufferData(OpenGLID, data.Count, data.Elements, GL_STATIC_DRAW);
	}

	void RStaticBuffer<GAPIType::OpenGL>::Update(UInt targetOffset, Splice<Byte> data)
	{
		glNamedBufferSubData(OpenGLID, targetOffset, data.Count, data.Elements);
	}
}