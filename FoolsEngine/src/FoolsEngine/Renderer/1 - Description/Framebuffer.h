#pragma once

#include "Texture.h"
#include "FoolsEngine\Core\UUID.h"

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
			UUID UUID;
			Texture::Type Type;
			uint32_t Width;
			uint32_t Height;
			uint32_t Samples;
			bool SwapChainTarget;
			Texture::Format DepthStencilFormat;
			std::pmr::vector<Attachment> ColorAttachments;

			Specification();
		};

		////TO DO: get rid of this
		//class SpecificationBuilder
		//{
		//public:
		//	SpecificationBuilder() = default;
		//
		//	SpecificationBuilder& SetWidth(uint32_t width)  { Width  = width;  return *this; }
		//	SpecificationBuilder& SetHight(uint32_t height) { Height = height; return *this; }
		//	SpecificationBuilder& SetSamples(uint32_t samples) { m_Samples = samples; return *this; }
		//	SpecificationBuilder& SetSwapChainTarget(bool isTarget) { m_SwapChainTarget = isTarget; return *this;	}
		//	SpecificationBuilder& SetDepthStencilAttachmentFormat(Texture::Format format) { DepthStencilFormat = format; return *this; }
		//	SpecificationBuilder& SetColorAttachmentSpecifications(const std::initializer_list<Attachment>& elements) { ColorAttachmentOpenGLIDs = elements; return *this; }
		//	SpecificationBuilder& AddColorAttachmentSpecification(Attachment specification) { ColorAttachmentOpenGLIDs.push_back(specification); return *this; }
		//
		//	Specification Create()
		//	{
		//		Specification spec;
		//
		//		spec.Width  = Width;
		//		spec.Height = Height;
		//
		//		spec.Samples = m_Samples;
		//		spec.SwapChainTarget = m_SwapChainTarget;
		//
		//		spec.DepthStencilFormat	= DepthStencilFormat;
		//		spec.ColorAttachments	= ColorAttachmentOpenGLIDs;
		//
		//		return spec;
		//	}
		//private:
		//	uint32_t Width   = 0;
		//	uint32_t Height  = 0;
		//	uint32_t m_Samples = 1;
		//	bool     m_SwapChainTarget = false;
		//
		//	Texture::Format DepthStencilFormat = Texture::Format::None;
		//	std::pmr::vector<Attachment> ColorAttachmentOpenGLIDs;
		//};
	}
}