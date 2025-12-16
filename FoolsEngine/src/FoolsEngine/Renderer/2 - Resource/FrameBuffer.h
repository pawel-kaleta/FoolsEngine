#pragma once

#include "FoolsEngine\Renderer\1 - Description\Framebuffer.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description::Framebuffer;

	struct FramebufferBase
	{
		uint32_t SpecificationID;

		virtual void Create() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		uint32_t GetColorAttachmentIndex(const std::string& name) const;
		virtual void Destroy() = 0;

	};

	struct Framebuffer_OpenGL final : FramebufferBase
	{
		GLuint OpenGLID;

		GLuint DepthStencilAttachmentOpenGLID;
		std::vector<GLuint> ColorAttachmentOpenGLIDs;

		virtual void Create() override;
		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void Destroy() override;
	};

	
}