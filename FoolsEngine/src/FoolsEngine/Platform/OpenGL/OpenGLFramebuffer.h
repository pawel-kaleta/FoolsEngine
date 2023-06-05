#pragma once
#include "FoolsEngine\Renderer\APIAbstraction\Framebuffer.h"

namespace fe
{
    class OpenGLFramebuffer : public Framebuffer
    {
    public:
        OpenGLFramebuffer(const FramebufferSpecification& spec);
        virtual ~OpenGLFramebuffer() override;

        virtual void Recreate(const FramebufferSpecification& spec) override;
        virtual void Recreate() override;
        virtual void Resize(uint32_t width, uint32_t height) override;
                
        virtual void Bind() override;
        virtual void Unbind() override;

        virtual const uint32_t GetColorAttachmentID() const override;

        virtual const FramebufferSpecification& GetSpecification() const override;

    private:
        uint32_t m_ID;
        uint32_t m_ColorAttachment, m_DepthAttachment;
        FramebufferSpecification m_Specification;

        void Create();
        void Delete();
    };


}