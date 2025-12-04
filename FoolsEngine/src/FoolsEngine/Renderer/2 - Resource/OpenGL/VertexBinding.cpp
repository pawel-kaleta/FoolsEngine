#include "FE_pch.h"

#include "Utils.h"

#include "FoolsEngine\Renderer\2 - Resource\VertexBinding.h"
#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include <glm\gtc\type_ptr.hpp>

namespace fe::Resource
{
	using namespace Description;

	void VertexBinding_OpenGL::Create()
	{
		FE_PROFILER_FUNC();

		glCreateVertexArrays(1, &VertexArrayID);
	}

	void VertexBinding_OpenGL::Delete()
	{
		FE_PROFILER_FUNC();

		glDeleteVertexArrays(1, &VertexArrayID);
	}
}