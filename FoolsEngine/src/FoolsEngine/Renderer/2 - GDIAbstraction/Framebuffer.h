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

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual const uint32_t GetColorAttachmentID() const = 0;

		virtual const FramebufferData::Specification& GetSpecification() const = 0;

		static Scope<Framebuffer> Create(const FramebufferData::Specification& spec);
	};
}