#pragma once

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/2 - Resource/Texture.h"
#include "FoolsEngine/Renderer/2 - Resource/VertexArray.h"

namespace fe::Command
{
	namespace Render
	{
		namespace Vulkan
		{
			void DrawIndexed(const Resource::VertexArray_Vulkan& vertexArray) {}
		}

		namespace OpenGL
		{
			void DrawIndexed(const Resource::VertexArray_OpenGL& vertexArray);
		}

		template<GAPIType::ValueType GAPI>
		void DrawIndexed(const Resource::VertexArrayBase& vertexArray)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::DrawIndexed(*(Resource::VertexArray_OpenGL*)&vertexArray);
			if constexpr (GAPI == GAPIType::Vulkan) Vulkan::DrawIndexed(*(Resource::VertexArray_Vulkan*)&vertexArray);
		}

	}
}