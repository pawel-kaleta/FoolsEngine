#include "FE_pch.h"

#include "Utils.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/2 - Resource/RFramebuffer.h"
#include "FoolsEngine/Foundation/Memory/Allocators/Allocator.h"
#include "FoolsEngine/Foundation/Utils/Context.h"

#include <glm/gtc/type_ptr.hpp>

namespace fe::Resource
{
	using namespace Description::Texture;

	void RFramebuffer<GAPIType::OpenGL>::Create()
	{
		FE_PROFILER_FUNC();

		const auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];

		glCreateFramebuffers(1, &OpenGLID);

		bool multisampled = spec.Samples > 1;
		UInt count = spec.ColorAttachments.Count;

		if (count)
		{
			auto alloc = Context::Allocators::Default;
			ColorAttachmentOpenGLIDs = alloc->Allocate(ColorAttachmentOpenGLIDs, count);

			if (multisampled)
			{
				FE_CORE_ASSERTION_BREAK(false, "Multisampled framebuffer not supported yet because multisample textures dont support DSA OpenGL");

				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, (GLsizei)count, ColorAttachmentOpenGLIDs.Elements);

				for (int i = 0; i < count; ++i)
				{
					GLenum internalFormat = Utils::FormatToGLInternalFormat(spec.ColorAttachments[i].Format);
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ColorAttachmentOpenGLIDs[i]);
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, internalFormat, spec.Width, spec.Height, GL_FALSE);
					glNamedFramebufferTexture(OpenGLID, GL_COLOR_ATTACHMENT0 + i, ColorAttachmentOpenGLIDs[i], 0);
				}
			}
			else
			{
				glCreateTextures(GL_TEXTURE_2D, (GLsizei)count, ColorAttachmentOpenGLIDs.Elements);

				for (int i = 0; i < count; ++i)
				{
					GLenum internalFormat = Utils::FormatToGLInternalFormat(spec.ColorAttachments[i].Format);
					GLuint textureID = ColorAttachmentOpenGLIDs[i];
					glTextureStorage2D(textureID, 1, internalFormat, spec.Width, spec.Height);
					glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTextureParameteri(textureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glNamedFramebufferTexture(OpenGLID, GL_COLOR_ATTACHMENT0 + i, textureID, 0);
				}
			}
		}

		if (spec.DepthStencilFormat != Description::Texture::Format::None)
		{
			GLenum dataFormat = Utils::FormatToGLInternalFormat(spec.DepthStencilFormat);

			if (multisampled)
			{
				FE_CORE_ASSERTION_BREAK(false, "Multisampled framebuffer not supported yet because multisample textures dont support DSA OpenGL");

				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &DepthStencilAttachmentOpenGLID);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, DepthStencilAttachmentOpenGLID);
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, dataFormat, spec.Width, spec.Height, GL_FALSE);
				glNamedFramebufferTexture(OpenGLID, GL_DEPTH_STENCIL_ATTACHMENT, DepthStencilAttachmentOpenGLID, 0);
			}
			else
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &DepthStencilAttachmentOpenGLID);
				GLuint textureID = DepthStencilAttachmentOpenGLID;
				glTextureStorage2D(textureID, 1, dataFormat, spec.Width, spec.Height);
				glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTextureParameteri(textureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glNamedFramebufferTexture(OpenGLID, GL_DEPTH_STENCIL_ATTACHMENT, DepthStencilAttachmentOpenGLID, 0);
			}
		}

		if (count > 1)
		{
			auto buffers = Pile().Allocate<GLenum>(count);

			for (int i = 0; i < count; ++i)
			{
				buffers[i] = GL_COLOR_ATTACHMENT0 + i;
			}
			glNamedFramebufferDrawBuffers(OpenGLID, (GLsizei)count, buffers.Elements);
		}
		else if (count)
		{
			glDrawBuffer(GL_NONE);
		}

		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
			FE_LOG_CORE_ERROR("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
		if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
			FE_LOG_CORE_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
		if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
			FE_LOG_CORE_ERROR("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
		if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
			FE_LOG_CORE_ERROR("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
		if (status == GL_FRAMEBUFFER_UNSUPPORTED)
			FE_LOG_CORE_ERROR("GL_FRAMEBUFFER_UNSUPPORTED");

		FE_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete.");
	}

	void RFramebuffer<GAPIType::OpenGL>::Resize(U32 width, U32 height)
	{
		FE_PROFILER_FUNC();

		FE_LOG_CORE_WARN("Framebuffer resizeing modifies specification!");

		auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];

		FE_CORE_ASSERT(spec.DynamicSize, "This framebuffer should not resized");

		spec.Width = width;
		spec.Height = height;

		bool multisampled = spec.Samples > 1;
		UInt count = spec.ColorAttachments.Count;

		if (count)
			glDeleteTextures((GLsizei)count, ColorAttachmentOpenGLIDs.Elements);
		if (DepthStencilAttachmentOpenGLID)
			glDeleteTextures(1, &DepthStencilAttachmentOpenGLID);

		if (count)
		{
			if (multisampled)
			{
				FE_CORE_ASSERTION_BREAK(false, "Multisampled framebuffer not supported yet because multisample textures dont support DSA OpenGL");
				
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, (GLsizei)count, ColorAttachmentOpenGLIDs.Elements);

				for (int i = 0; i < count; ++i)
				{
					GLenum internalFormat = Utils::FormatToGLInternalFormat(spec.ColorAttachments[i].Format);
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ColorAttachmentOpenGLIDs[i]);
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, internalFormat, width, height, GL_FALSE);

					glNamedFramebufferTexture(OpenGLID, GL_COLOR_ATTACHMENT0 + i, ColorAttachmentOpenGLIDs[i], 0);
				}
			}
			else
			{
				glCreateTextures(GL_TEXTURE_2D, (GLsizei)count, ColorAttachmentOpenGLIDs.Elements);

				for (int i = 0; i < count; ++i)
				{
					GLenum internalFormat = Utils::FormatToGLInternalFormat(spec.ColorAttachments[i].Format);
					GLuint textureID = ColorAttachmentOpenGLIDs[i];
					glTextureStorage2D(textureID, 1, internalFormat, width, height);
					glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTextureParameteri(textureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glNamedFramebufferTexture(OpenGLID, GL_COLOR_ATTACHMENT0 + i, textureID, 0);
				}
			}
		}

		if (spec.DepthStencilFormat != Description::Texture::Format::None)
		{
			GLenum dataFormat = Utils::FormatToGLInternalFormat(spec.DepthStencilFormat);

			if (multisampled)
			{
				FE_CORE_ASSERTION_BREAK(false, "Multisampled framebuffer not supported yet because multisample textures dont support DSA OpenGL");

				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &DepthStencilAttachmentOpenGLID);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, DepthStencilAttachmentOpenGLID);
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, dataFormat, width, height, GL_FALSE);
				glNamedFramebufferTexture(OpenGLID, GL_DEPTH_STENCIL_ATTACHMENT, DepthStencilAttachmentOpenGLID, 0);
			}
			else
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &DepthStencilAttachmentOpenGLID);
				GLuint textureID = DepthStencilAttachmentOpenGLID;
				glTextureStorage2D(textureID, 1, dataFormat, width, height);
				glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTextureParameteri(textureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glNamedFramebufferTexture(OpenGLID, GL_DEPTH_STENCIL_ATTACHMENT, textureID, 0);
			}
		}
	}

	void RFramebuffer<GAPIType::OpenGL>::Destroy()
	{
		FE_PROFILER_FUNC();

		glDeleteFramebuffers(1, &OpenGLID);
		UInt count = ColorAttachmentOpenGLIDs.Count;
		if (count)
		{
			glDeleteTextures((GLsizei)count, ColorAttachmentOpenGLIDs.Elements);
			auto alloc = Context::Allocators::Default;
			alloc->Deallocate(ColorAttachmentOpenGLIDs);
		}
		if (DepthStencilAttachmentOpenGLID)
			glDeleteTextures(1, &DepthStencilAttachmentOpenGLID);

	}

	void RFramebuffer<GAPIType::OpenGL>::ReadPixel(UInt attachmentIndex, UInt x, UInt y, Splice<Byte> destination)
	{
		FE_PROFILER_FUNC();

		glNamedFramebufferReadBuffer(OpenGLID, GL_COLOR_ATTACHMENT0 + attachmentIndex);

		const auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];
		auto& format = spec.ColorAttachments[attachmentIndex].Format;

		FE_CORE_ASSERT(Description::Texture::SizeOfFormat(format) == destination.Count, "Format size and destination capacity!");

		GLenum glFormat = Resource::Utils::FormatToGLFormat(format);
		GLenum glType = Resource::Utils::FormatToGLType(format);
		glReadPixels((GLint)x, (GLint)y, 1, 1, glFormat, glType, destination.Elements);
	}

	void RFramebuffer<GAPIType::OpenGL>::ClearAttachment(UInt attachmentIndex, Splice<U32> values)
	{
		FE_PROFILER_FUNC();

		const auto& attachmentID = ColorAttachmentOpenGLIDs[attachmentIndex];
		const auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];
		const auto& format = spec.ColorAttachments[attachmentIndex].Format;

		FE_CORE_ASSERT(!Description::Texture::IsDepthOrStencil(format), "This is a depth and/or stencil texture!");
		FE_CORE_ASSERT(Description::Texture::ComponentsCountInFormat(format) == values.Count, "Mismach between number of provided values and number of components in color attachment!");

		glClearTexImage(attachmentID, 0, Resource::Utils::FormatToGLFormat(format), GL_UNSIGNED_INT, values.Elements);
	}

	void RFramebuffer<GAPIType::OpenGL>::ClearAttachment(UInt attachmentIndex, Splice<float> values)
	{
		FE_PROFILER_FUNC();

		const auto& attachmentID = ColorAttachmentOpenGLIDs[attachmentIndex];
		const auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];
		const auto& format = spec.ColorAttachments[attachmentIndex].Format;

		FE_CORE_ASSERT(!Description::Texture::IsDepthOrStencil(format), "This is a depth and/or stencil texture!");
		FE_CORE_ASSERT(Description::Texture::ComponentsCountInFormat(format) == values.Count, "Mismach between number of provided values and number of components in color attachment!");

		glClearTexImage(attachmentID, 0, Resource::Utils::FormatToGLFormat(format), GL_FLOAT, values.Elements);
	}


}