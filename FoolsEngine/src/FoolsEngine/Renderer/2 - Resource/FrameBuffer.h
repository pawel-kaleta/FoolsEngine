#pragma once

#include "FoolsEngine\Renderer\1 - Description\Framebuffer.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description::Framebuffer;

	struct FramebufferBase
	{
		uint32_t m_SpecificationID;
		uint32_t m_Width;
		uint32_t m_Height;

		virtual void Create() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
	};

	struct Framebuffer_OpenGL final : FramebufferBase
	{
		GLuint m_OpenGLID;

		GLuint m_DepthStencilAttachment;
		std::vector<GLuint> m_ColorAttachments;

		virtual void Create() override;
		virtual void Resize(uint32_t width, uint32_t height) override;
	};
}