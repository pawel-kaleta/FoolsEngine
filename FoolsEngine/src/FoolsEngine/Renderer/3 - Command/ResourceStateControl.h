#pragma once

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\1 - Description\ShaderInterface.h"
#include "FoolsEngine\Renderer\2 - Resource\Texture.h"
#include "FoolsEngine\Renderer\2 - Resource\VertexBuffer.h"
#include "FoolsEngine\Renderer\2 - Resource\VertexBinding.h"
#include "FoolsEngine\Renderer\2 - Resource\IndexBuffer.h"

namespace fe::Command
{
	namespace ResourceStateControl
	{
		namespace Vulkan
		{
			void BindToRendererTextureSlot(uint32_t rendererTextureSlot, const Resource::TextureBase& texture) {}
		}

		namespace OpenGL
		{
			void BindToRendererTextureSlot(uint32_t rendererTextureSlot, const Resource::Texture_OpenGL& texture);
			void BindVertexBuffer(const Resource::VertexBuffer_OpenGL& vertexBuffer);
			void BindVertexBinding(const Resource::VertexBinding_OpenGL& vertexBinding);
			void BindIndexBuffer(const Resource::IndexBuffer_OpenGL& indexBuffer);
		}

		template<GAPIType::ValueType GAPI>
		void BindToRendererTextureSlot(uint32_t rendererTextureSlot, const Resource::TextureBase& texture)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::BindToRendererTextureSlot(rendererTextureSlot, * (const Resource::Texture_OpenGL *) & texture);
			if constexpr (GAPI == GAPIType::Vulkan) Vulkan::BindToRendererTextureSlot(rendererTextureSlot, texture);
		}

		template<GAPIType::ValueType GAPI>
		void BindVertexBuffer(const Resource::VertexBufferBase& vertexBuffer)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::BindVertexBuffer( * (const Resource::VertexBuffer_OpenGL *) & vertexBuffer);
		}

		template<GAPIType::ValueType GAPI>
		void BindVertexBinding(const Resource::VertexBindingBase& vertexBinding)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::BindVertexBinding( * (const Resource::VertexBinding_OpenGL *) & vertexBinding);
		}

		template<GAPIType::ValueType GAPI>
		void BindIndexBuffer(const Resource::IndexBufferBase& indexBuffer)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::BindIndexBuffer( * (const Resource::IndexBuffer_OpenGL *) & indexBuffer);
		}
	}
}