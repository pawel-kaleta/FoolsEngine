#pragma once

#include "FoolsEngine\Renderer\1 - Description\Framebuffer.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description::Framebuffer;

	struct FramebufferBase
	{
		uint32_t SpecificationID;
		uint32_t Width;
		uint32_t Height;

		virtual void Create() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		uint32_t GetColorAttachmentIndex(const std::string& name) const;
		virtual void ClearAttachment(uint32_t attachmentIndex, uint32_t value) = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, float value) = 0;
		virtual void ReadPixel(uint32_t attachmentIndex, int x, int y, void* destination) const = 0;
		virtual void Destroy() = 0;

	};

	struct Framebuffer_OpenGL final : FramebufferBase
	{
		GLuint FramebufferOpenGLID;

		GLuint DepthStencilAttachmentOpenGLID;
		std::vector<GLuint> ColorAttachmentOpenGLIDs;

		virtual void Create() override;
		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void ClearAttachment(uint32_t attachmentIndex, uint32_t value) override;
		virtual void ClearAttachment(uint32_t attachmentIndex, float value) override;
		virtual void ReadPixel(uint32_t attachmentIndex, int x, int y, void* destination) const override;
		virtual void Destroy() override;
	};

	
}