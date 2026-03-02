#pragma once

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\1 - Description\Pipeline.h"
#include "FoolsEngine\Renderer\2 - Resource\Texture.h"
#include "FoolsEngine\Renderer\2 - Resource\VertexArray.h"
#include "FoolsEngine\Renderer\2 - Resource\FrameBuffer.h"

namespace fe::Command
{
	namespace PipelineState
	{
		namespace Vulkan
		{
			void BindVertexArray(const Resource::VertexArray_Vulkan& vertexBinding) {}
		}

		namespace OpenGL
		{
			void BindVertexArray(const Resource::VertexArray_OpenGL& vertexBinding);

			void BindTextureToRendererTextureSlot(uint32_t rendererTextureSlot, const Resource::Texture_OpenGL& texture);

			void SetDepthTest(bool enable);

			void SetDepthTestType(Description::Pipeline::DepthTestType type);

			void SetBlendFunction(Description::Pipeline::BlendFunction source, Description::Pipeline::BlendFunction destination);

			void SetBlending(bool enable);

			void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

			void BindFramebuffer(const Resource::Framebuffer_OpenGL& framebuffer);
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

		template<GAPIType::ValueType GAPI>
		void SetDepthTest(bool enable)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::SetDepthTest(enable);
		}

		template<GAPIType::ValueType GAPI>
		void SetDepthTestType(Description::Pipeline::DepthTestType type)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::SetDepthTestType(type);
		}

		template<GAPIType::ValueType GAPI>
		void SetBlending(bool enable)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::SetBlending(enable);
		}

		template<GAPIType::ValueType GAPI>
		void SetBlendFunction(Description::Pipeline::BlendFunction source, Description::Pipeline::BlendFunction destination)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::SetBlendFunction(source, destination);
		}

		template<GAPIType::ValueType GAPI>
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::SetViewport(x, y, width, height);
		}

		template<GAPIType::ValueType GAPI>
		void BindFramebuffer(const Resource::FramebufferBase& framebuffer)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::BindFramebuffer((const Resource::Framebuffer_OpenGL &)  framebuffer);
		}
	}
}