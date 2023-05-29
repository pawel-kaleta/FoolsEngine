#pragma once
#include "FoolsEngine\Renderer\APIAbstraction\Framebuffer.h"

namespace fe
{
    class OpenGLFramebuffer : public Framebuffer
    {
    public:
        OpenGLFramebuffer(const FramebufferSpecification& spec);
        virtual ~OpenGLFramebuffer() override;

        void Recreate(const FramebufferSpecification& spec);
        void Recreate();
                
        virtual void Bind() override;
        virtual void Unbind() override;

        virtual const uint32_t GetColorAttachmentID() const override;

        virtual const FramebufferSpecification& GetSpecification() const override;

    private:
        uint32_t m_ID;
        uint32_t m_ColorAttachment, m_DepthAttachment;
        FramebufferSpecification m_Specification;
    };


}