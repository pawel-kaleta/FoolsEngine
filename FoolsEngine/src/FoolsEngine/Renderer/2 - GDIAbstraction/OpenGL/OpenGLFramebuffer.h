#pragma once
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Framebuffer.h"

#include <glad\glad.h>

namespace fe
{
    class OpenGLFramebuffer : public Framebuffer
    {
    public:
        OpenGLFramebuffer(const FramebufferData::Specification& spec);
        virtual ~OpenGLFramebuffer() override;

        virtual void Recreate(const FramebufferData::Specification& spec) override;
        virtual void Recreate() override;
        virtual void Resize(uint32_t width, uint32_t height) override;
                
        virtual void Bind() override;
        virtual void Unbind() override;

        virtual uint32_t GetDepthAttachmentID() const override { return m_DepthAttachment; }
        virtual uint32_t GetColorAttachmentID(uint32_t index = 0) const override
        {
            FE_CORE_ASSERT(index < m_ColorAttachments.size(), "Color Attachment index aout of bounds");
            return m_ColorAttachments[index];
        }

        virtual const FramebufferData::Specification& GetSpecification() const override { return m_Specification; };

        static GLenum FormatToGLenum(TextureData::Format format);
        static GLenum DataFormatToGLenum(TextureData::DataFormat dataFormat);
    private:
        uint32_t m_ID = 0;
        FramebufferData::Specification m_Specification;

        uint32_t m_DepthAttachment = 0;
        std::vector<uint32_t> m_ColorAttachments;

        void Create();
        void Delete();
    };


}