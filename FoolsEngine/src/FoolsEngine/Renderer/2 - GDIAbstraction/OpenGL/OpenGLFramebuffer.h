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
                
        virtual void Bind()   override;
        virtual void Unbind() override;

        virtual const std::string& GetName() const override { return m_Name; }
        virtual void               SetName(const std::string& name) override { m_Name = name; }

        virtual uint32_t GetDepthStencilAttachmentID() const override { return m_DepthStencilAttachment; }
        virtual uint32_t GetColorAttachmentID(uint32_t index = 0) const override
        {
            FE_CORE_ASSERT(index < m_ColorAttachments.size(), "Attachment index out of bounds");
            return m_ColorAttachments[index];
        }
        virtual uint32_t GetColorAttachmentID(const std::string& name) const override
        {
            for (int i = 0; i < m_Specification.ColorAttachments.size(); ++i)
            {
                if (m_Specification.ColorAttachments[i].Name == name)
                    return m_ColorAttachments[i];
            }
            FE_CORE_ASSERT(false, "Attachment {0} not found in framebuffer {1}", name, m_Name);
        }

        virtual const FramebufferData::Specification& GetSpecification() const override { return m_Specification; };

        static GLenum FormatToGLenum(TextureData::Components format);
        static GLenum DataFormatToGLenum(TextureData::Format dataFormat);
    private:
        uint32_t m_ID = 0;
        std::string m_Name;
        FramebufferData::Specification m_Specification;

        uint32_t m_DepthStencilAttachment = 0;
        std::vector<uint32_t> m_ColorAttachments;

        void Create();
        void Delete();
    };


}