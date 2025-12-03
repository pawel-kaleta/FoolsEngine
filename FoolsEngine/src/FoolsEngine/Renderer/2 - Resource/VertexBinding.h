#pragma once

#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description;

	struct VertexBindingBase
	{
		uint32_t m_LayoutID;

		virtual void Create() = 0;
		virtual void Delete() = 0;
	};

	struct VertexBinding_OpenGL final : VertexBindingBase
	{
		GLuint m_VertexArrayID;

		virtual void Create() override;
		virtual void Delete() override;
	};
}