#pragma once

#include "TextureData.h"

namespace fe
{
	namespace FramebufferData
	{
		struct AttachmentSpecification
		{
			std::string             Name;
			TextureData::Components Components;
			TextureData::Format     Format;
		};

		struct Specification
		{
			uint32_t Width = 0, Height = 0;

			uint32_t Samples = 1;

			bool SwapChainTarget = false;


			TextureData::Format DepthStencilAttachment = TextureData::Format::None;
			std::vector<AttachmentSpecification> ColorAttachments;
		};

		class SpecificationBuilder
		{
		public:
			SpecificationBuilder() = default;

			SpecificationBuilder& SetWidth(uint32_t width)  { m_Width  = width;  return *this; }
			SpecificationBuilder& SetHight(uint32_t height) { m_Height = height; return *this; }
			SpecificationBuilder& SetSamples(uint32_t samples) { m_Samples = samples; return *this; }
			SpecificationBuilder& SetSwapChainTarget(bool isTarget) { m_SwapChainTarget = isTarget; return *this;	}
			SpecificationBuilder& SetDepthStencilAttachmentDataFormat(TextureData::Format dataFormat) { m_DepthStencilAttachment = dataFormat; return *this; }
			SpecificationBuilder& SetColorAttachmentSpecifications(const std::initializer_list<AttachmentSpecification>& elements) { m_ColorAttachments = elements; return *this; }
			SpecificationBuilder& AddColorAttachmentSpecification(AttachmentSpecification specification) { m_ColorAttachments.push_back(specification); return *this; }

			Specification Create()
			{
				Specification spec;

				spec.Width  = m_Width;
				spec.Height = m_Height;

				spec.Samples = m_Samples;
				spec.SwapChainTarget = m_SwapChainTarget;

				spec.DepthStencilAttachment = m_DepthStencilAttachment;
				spec.ColorAttachments       = m_ColorAttachments;

				return spec;
			}
		private:
			uint32_t m_Width   = 0;
			uint32_t m_Height  = 0;
			uint32_t m_Samples = 1;
			bool     m_SwapChainTarget = false;

			TextureData::Format m_DepthStencilAttachment = TextureData::Format::None;
			std::vector<AttachmentSpecification> m_ColorAttachments;
		};
	}
}