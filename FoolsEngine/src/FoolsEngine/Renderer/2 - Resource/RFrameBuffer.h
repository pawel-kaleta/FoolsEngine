#pragma once

#include "FoolsEngine/Renderer/1 - Description/Framebuffer.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Foundation/Common.h"

#include <glad/glad.h>

namespace fe::Resource
{
	using namespace Description::Framebuffer;

	struct FramebufferBase
	{
		UInt SpecificationID;

		virtual void Create() = 0;
		virtual void Resize(U32 width, U32 height) = 0;
		UInt GetColorAttachmentIndex(String name) const;
		virtual void ReadPixel(UInt attachmentIndex, UInt x, UInt y, Splice<Byte> destination) = 0;
		virtual void ClearAttachment(UInt attachmentIndex, Splice<U32> values) = 0;
		virtual void ClearAttachment(UInt attachmentIndex, Splice<float> values) = 0;

		virtual void Destroy() = 0;

	};

	template <GAPIType::ValueType GAPI>
	struct RFramebuffer;

	template <>
	struct RFramebuffer<GAPIType::OpenGL> final : public FramebufferBase
	{
		GLuint OpenGLID;

		GLuint DepthStencilAttachmentOpenGLID;
		Splice<GLuint> ColorAttachmentOpenGLIDs;

		virtual void Create() final override;
		virtual void Resize(U32 width, U32 height) final override;
		virtual void ReadPixel(UInt attachmentIndex, UInt x, UInt y, Splice<Byte> destination) final override;
		virtual void ClearAttachment(UInt attachmentIndex, Splice<U32> values) final override;
		virtual void ClearAttachment(UInt attachmentIndex, Splice<float> values) final override;

		virtual void Destroy() final override;
	};

	using RFramebuffer_OpenGL = RFramebuffer<GAPIType::OpenGL>;
}