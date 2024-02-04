#pragma once

#include "TextureData.h"

namespace fe
{
	namespace FramebufferData
	{
		struct Specification
		{
			uint32_t Width = 0, Height = 0;

			uint32_t Samples = 1;

			bool SwapChainTarget = false;

			TextureData::DataFormat DepthAttachment = TextureData::DataFormat::None;
			std::vector<TextureData::Specification> ColorAttachments;
		};

		class SpecificationBuilder
		{
		public:
			SpecificationBuilder() = default;

			SpecificationBuilder& SetWidth(uint32_t width)  { m_Width  = width;  return *this; }
			SpecificationBuilder& SetHight(uint32_t height) { m_Height = height; return *this; }
			SpecificationBuilder& SetSamples(uint32_t samples) { m_Samples = samples; return *this; }
			SpecificationBuilder& SetSwapChainTarget(bool isTarget) { m_SwapChainTarget = isTarget; return *this;	}
			SpecificationBuilder& SetDepthAttachmentDataFormat(TextureData::DataFormat dataFormat) { m_DepthAttachment = dataFormat; return *this; }
			SpecificationBuilder& SetCollorAttachmentSpecifications(const std::initializer_list<TextureData::Specification>& elements) { m_ColorAttachments = elements; return *this; }
			SpecificationBuilder& AddColorAttachmentSpecification(TextureData::Specification specification) { m_ColorAttachments.push_back(specification); return *this; }

			Specification Create()
			{
				Specification spec;

				spec.Width            = m_Width;
				spec.Height           = m_Height;
				spec.DepthAttachment  = m_DepthAttachment;
				spec.Samples          = m_Samples;
				spec.SwapChainTarget  = m_SwapChainTarget;
				spec.ColorAttachments = m_ColorAttachments;

				return spec;
			}
		private:
			uint32_t m_Width   = 0;
			uint32_t m_Height  = 0;
			uint32_t m_Samples = 1;
			bool     m_SwapChainTarget = false;

			TextureData::DataFormat m_DepthAttachment = TextureData::DataFormat::None;
			std::vector<TextureData::Specification> m_ColorAttachments;
		};
	}
}