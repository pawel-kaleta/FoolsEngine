#pragma once

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\2 - Resource\Texture.h"
#include "FoolsEngine\Renderer\2 - Resource\VertexArray.h"

namespace fe::Command
{
	namespace DeviceStateControl
	{
		namespace Vulkan
		{
			void BindVertexArray(const Resource::VertexArray_Vulkan& vertexBinding) {}
		}

		namespace OpenGL
		{
			void BindVertexArray(const Resource::VertexArray_OpenGL& vertexBinding);

			void BindTextureToRendererTextureSlot(uint32_t rendererTextureSlot, const Resource::Texture_OpenGL& texture);
		}

		template<GAPIType::ValueType GAPI>
		void BindVertexArray(const Resource::VertexArrayBase& vertexBinding)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::BindVertexArray(*(Resource::VertexArray_OpenGL*)&vertexBinding);
			if constexpr (GAPI == GAPIType::Vulkan) Vulkan::BindVertexArray(*(Resource::VertexArray_Vulkan*)&vertexBinding);
		}

		template<GAPIType::ValueType GAPI>
		void BindTextureToRendererTextureSlot(uint32_t rendererTextureSlot, const Resource::TextureBase& texture)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::BindVertexArray(*(Resource::Texture_OpenGL*)&texture);
		}
	}
}