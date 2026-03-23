#pragma once

#include "FoolsEngine/Renderer/1 - Description/Framebuffer.h"
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
		UInt GetColorAttachmentIndex(const String& name) const;
		virtual void Destroy() = 0;

	};

	struct Framebuffer_OpenGL final : FramebufferBase
	{
		GLuint OpenGLID;

		GLuint DepthStencilAttachmentOpenGLID;
		Splice<GLuint> ColorAttachmentOpenGLIDs;

		virtual void Create() override;
		virtual void Resize(U32 width, U32 height) override;
		virtual void Destroy() override;
	};

	
}