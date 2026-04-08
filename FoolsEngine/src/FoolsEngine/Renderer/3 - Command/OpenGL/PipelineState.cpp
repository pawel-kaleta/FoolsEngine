#include "FE_pch.h"

#include "FoolsEngine/Renderer/2 - Resource/OpenGL/Utils.h"
#include "FoolsEngine/Renderer/3 - Command/PipelineState.h"

namespace fe::Command
{
	namespace PipelineState
	{
		template <> void BindFramebuffer<GAPIType::OpenGL>(const Resource::RFramebuffer<GAPIType::OpenGL>& framebuffer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.OpenGLID);
			const auto& spec = Description::Library::Get().FramebufferSpecs[framebuffer.SpecificationID];
			glViewport(0, 0, spec.Width, spec.Height); // ??-> SetViewport()
		}

		template <> void BindMeshBindings<GAPIType::OpenGL>(const Resource::RMeshBindings<GAPIType::OpenGL>& meshBindings)
		{
			glBindVertexArray(meshBindings.OpenGLID);
		}

		template <> void BindTextureToRendererTextureSlot<GAPIType::OpenGL>(U32 rendererTextureSlot, const Resource::RTexture<GAPIType::OpenGL>& texture)
		{
			glBindTextureUnit(rendererTextureSlot, texture.OpenGLID);
		}

		template <> void SetDepthTest<GAPIType::OpenGL>(bool enable)
		{
			if (enable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}

		template <> void SetDepthTestType<GAPIType::OpenGL>(Description::Pipeline::DepthTestType type)
		{
			GLenum func = Resource::Utils::DepthTestTypeToGLEnum(type);
			glDepthFunc(func);
		}

		template <> void SetBlendFunction<GAPIType::OpenGL>(Description::Pipeline::BlendFunction source, Description::Pipeline::BlendFunction destination)
		{
			GLenum source_factor = Resource::Utils::BlendFunctionToGLEnum(source);
			GLenum destination_factor = Resource::Utils::BlendFunctionToGLEnum(source);
			glBlendFunc(source_factor, destination_factor);
		}

		template <> void SetBlending<GAPIType::OpenGL>(bool enable)
		{
			if (enable)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
		}

		template <> void SetViewport<GAPIType::OpenGL>(U32 x, U32 y, U32 width, U32 height)
		{
			glViewport(x, y, width, height); // ??-> BindFramebuffer()
		}
	}
}