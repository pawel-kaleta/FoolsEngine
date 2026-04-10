#include "FE_pch.h"

#include "FoolsEngine/Renderer/2 - Resource/OpenGL/Utils.h"
#include "FoolsEngine/Renderer/3 - Command/PipelineState.h"

namespace fe::Command
{
	void PipelineState_OpenGL::BindFramebuffer(const Resource::RFramebuffer& framebuffer)
	{
		const Resource::RFramebuffer_OpenGL& opengl_framebuffer = *(const Resource::RFramebuffer_OpenGL*) & framebuffer;
		glBindFramebuffer(GL_FRAMEBUFFER, opengl_framebuffer.OpenGLID);
		const auto& spec = Description::Library::Get().FramebufferSpecs[framebuffer.SpecificationID];
		glViewport(0, 0, spec.Width, spec.Height); // ??-> SetViewport()
	}

	void PipelineState_OpenGL::BindMeshBindings(const Resource::RMeshBindings& meshBindings)
	{
		const Resource::RMeshBindings_OpenGL& OpenGL_meshBindings = *(const Resource::RMeshBindings_OpenGL*) & meshBindings;
		glBindVertexArray(OpenGL_meshBindings.OpenGLID);
	}

	void PipelineState_OpenGL::BindTextureToRendererTextureSlot(U32 rendererTextureSlot, const Resource::RTexture& texture)
	{
		const Resource::RTexture_OpenGL& OpenGL_texture = *(const Resource::RTexture_OpenGL*) & texture;
		glBindTextureUnit(rendererTextureSlot, OpenGL_texture.OpenGLID);
	}

	void PipelineState_OpenGL::SetDepthTest(bool enable)
	{
		if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void PipelineState_OpenGL::SetDepthTestType(Description::Pipeline::DepthTestType type)
	{
		GLenum func = Resource::Utils::DepthTestTypeToGLEnum(type);
		glDepthFunc(func);
	}

	void PipelineState_OpenGL::SetBlendFunction(Description::Pipeline::BlendFunction source, Description::Pipeline::BlendFunction destination)
	{
		GLenum source_factor = Resource::Utils::BlendFunctionToGLEnum(source);
		GLenum destination_factor = Resource::Utils::BlendFunctionToGLEnum(source);
		glBlendFunc(source_factor, destination_factor);
	}

	void PipelineState_OpenGL::SetBlending(bool enable)
	{
		if (enable)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}

	void PipelineState_OpenGL::SetViewport(U32 x, U32 y, U32 width, U32 height)
	{
		glViewport(x, y, width, height); // ??-> BindFramebuffer()
	}
}