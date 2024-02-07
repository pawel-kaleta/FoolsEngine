#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\FramebufferData.h"

#include<glm\glm.hpp>

namespace fe
{
	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Recreate(const FramebufferData::Specification& spec) = 0;
		virtual void Recreate() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, uint32_t value) = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, float value) = 0;

		virtual void Bind()   = 0;
		virtual void Unbind() = 0;

		virtual const std::string& GetName() const = 0;
		virtual void               SetName(const std::string& name) = 0;

		virtual uint32_t GetDepthStencilAttachmentID()                    const = 0;
		virtual uint32_t GetColorAttachmentID(uint32_t index = 0)         const = 0;
		virtual uint32_t GetColorAttachmentIndex(const std::string& name) const = 0;

		virtual void ReadPixel(uint32_t attachmentIndex, int x, int y, void* saveLocation) = 0;

		virtual const FramebufferData::Specification& GetSpecification() const = 0;

		static Scope<Framebuffer> Create(const FramebufferData::Specification& spec);
	};
}