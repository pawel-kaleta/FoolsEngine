#pragma once
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Framebuffer.h"

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

        virtual const uint32_t GetColorAttachmentID() const override;

        virtual const FramebufferData::Specification& GetSpecification() const override;

    private:
        uint32_t m_ID;
        uint32_t m_ColorAttachment, m_DepthAttachment;
        FramebufferData::Specification m_Specification;

        void Create();
        void Delete();
    };


}