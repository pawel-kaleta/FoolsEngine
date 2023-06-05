#pragma once

#include<glm\glm.hpp>

namespace fe
{
	struct FramebufferSpecification
	{
		uint32_t Width, Height;

		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Recreate(const FramebufferSpecification& spec) = 0;
		virtual void Recreate() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual const uint32_t GetColorAttachmentID() const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Scope<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}