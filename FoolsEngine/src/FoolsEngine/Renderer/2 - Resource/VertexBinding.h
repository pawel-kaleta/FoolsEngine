#pragma once

#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct VertexBindingBase
	{
		uint32_t LayoutID;

		virtual void Create() = 0;
		virtual void Delete() = 0;
	};

	struct VertexBinding_OpenGL final : VertexBindingBase
	{
		GLuint VertexArrayID;

		virtual void Create() override;
		virtual void Delete() override;
	};
}