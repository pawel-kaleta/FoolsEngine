#include "FE_pch.h"

#include "FoolsEngine\Renderer\3 - Command\DeviceState.h"
#include "FoolsEngine\Renderer\2 - Resource\OpenGL\Utils.h"

namespace fe::Command
{
	namespace DeviceState::OpenGL
	{
		void BindVertexArray(const Resource::VertexArray_OpenGL& vertexBinding)
		{
			glBindVertexArray(vertexBinding.OpenGLID);
		}

		void BindTextureToRendererTextureSlot(uint32_t rendererTextureSlot, const Resource::Texture_OpenGL& texture)
		{
			glBindTextureUnit(rendererTextureSlot, texture.OpenGLID);
		}

		void SetDepthTest(bool enable)
		{
			if (enable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}

		void SetDepthTestType(Description::Pipeline::DepthTestType type)
		{
			GLenum func = Resource::Utils::DepthTestTypeToGLEnum(type);
			glDepthFunc(func);
		}

		void SetBlending(bool enable)
		{
			if (enable)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
		}

		void SetBlendFunction(Description::Pipeline::BlendFunction source, Description::Pipeline::BlendFunction destination)
		{
			GLenum source_factor = Resource::Utils::BlendFunctionToGLEnum(source);
			GLenum destination_factor = Resource::Utils::BlendFunctionToGLEnum(source);
			glBlendFunc(source_factor, destination_factor);
		}

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			glViewport(x, y, width, height); // ??-> BindFramebuffer()
		}

		void BindFramebuffer(const Resource::Framebuffer_OpenGL& framebuffer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.OpenGLID);
			const auto& spec = Description::Library::Get().FramebufferSpecs[framebuffer.SpecificationID];
			glViewport(0, 0, spec.Width, spec.Height); // ??-> SetViewport()
		}
	}
}