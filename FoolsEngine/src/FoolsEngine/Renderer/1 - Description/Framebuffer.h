#pragma once

#include "Texture.h"

namespace fe
{
	namespace Description::Framebuffer
	{
		struct Attachment
		{
			std::pmr::string Name;
			Texture::Format Format;

			Attachment();
			Attachment(const std::pmr::string& name, Texture::Format format);
		};

		struct Specification
		{
			Texture::Type Type;
			uint32_t Width;
			uint32_t Height;
			uint32_t Samples;
			bool SwapChainTarget;
			Texture::Format DepthStencilFormat;
			std::pmr::vector<Attachment> ColorAttachments;

			Specification();
		};

		//TO DO: get rid of this
		class SpecificationBuilder
		{
		public:
			SpecificationBuilder() = default;

			SpecificationBuilder& SetWidth(uint32_t width)  { m_Width  = width;  return *this; }
			SpecificationBuilder& SetHight(uint32_t height) { m_Height = height; return *this; }
			SpecificationBuilder& SetSamples(uint32_t samples) { m_Samples = samples; return *this; }
			SpecificationBuilder& SetSwapChainTarget(bool isTarget) { m_SwapChainTarget = isTarget; return *this;	}
			SpecificationBuilder& SetDepthStencilAttachmentFormat(Texture::Format format) { DepthStencilFormat = format; return *this; }
			SpecificationBuilder& SetColorAttachmentSpecifications(const std::initializer_list<Attachment>& elements) { m_ColorAttachments = elements; return *this; }
			SpecificationBuilder& AddColorAttachmentSpecification(Attachment specification) { m_ColorAttachments.push_back(specification); return *this; }

			Specification Create()
			{
				Specification spec;

				spec.Width  = m_Width;
				spec.Height = m_Height;

				spec.Samples = m_Samples;
				spec.SwapChainTarget = m_SwapChainTarget;

				spec.DepthStencilFormat	= DepthStencilFormat;
				spec.ColorAttachments	= m_ColorAttachments;

				return spec;
			}
		private:
			uint32_t m_Width   = 0;
			uint32_t m_Height  = 0;
			uint32_t m_Samples = 1;
			bool     m_SwapChainTarget = false;

			Texture::Format DepthStencilFormat = Texture::Format::None;
			std::pmr::vector<Attachment> m_ColorAttachments;
		};
	}
}