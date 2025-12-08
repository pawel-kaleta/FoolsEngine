#pragma once

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"
#include "FoolsEngine\Renderer\2 - Resource\Program.h"
#include "FoolsEngine\Renderer\2 - Resource\Framebuffer.h"

namespace fe::Command
{
	namespace ResourceStateControl
	{
		namespace Vulkan
		{
			void BindTextureSamplerToRendererTextureSlot(Resource::Program_Vulkan& program, size_t samplerIndex, RenderTextureSlotID rendererTextureSlot) {}
		}

		namespace OpenGL
		{
			// shader interface setup

			void BindTextureSamplerToRendererTextureSlot(Resource::Program_OpenGL& program, size_t samplerIndex, RenderTextureSlotID rendererTextureSlot);

			void BindTextureSamplerToRendererTextureSlot(Resource::Program_OpenGL& program, const std::pmr::string& samplerName, RenderTextureSlotID rendererTextureSlot);

			void UploadUniform(Resource::Program_OpenGL& program, size_t uniformIndex, const void* data);

			void UploadUniform(Resource::Program_OpenGL& program, const std::pmr::string& uniformName, const void* data);

			// framebuffer

			void ClearAttachment(Resource::Framebuffer_OpenGL& framebuffer, uint32_t attachmentIndex, uint32_t value);

			void ClearAttachment(Resource::Framebuffer_OpenGL& framebuffer, uint32_t attachmentIndex, float value);

			void ReadPixel(const Resource::Framebuffer_OpenGL& framebuffer, uint32_t attachmentIndex, int x, int y, void* destination);
		}

		// shader interface setup

		template<GAPIType::ValueType GAPI>
		void BindTextureSamplerToRendererTextureSlot(Resource::ProgramBase& program, size_t samplerIndex, RenderTextureSlotID rendererTextureSlot)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::BindTextureSamplerToRendererTextureSlot(*(Resource::Program_OpenGL*) & program, samplerIndex, rendererTextureSlot);
			if constexpr (GAPI == GAPIType::Vulkan) Vulkan::BindTextureSamplerToRendererTextureSlot(*(Resource::Program_Vulkan*) & program, samplerIndex, rendererTextureSlot);
		}

		template<GAPIType::ValueType GAPI>
		void BindTextureSamplerToRendererTextureSlot(Resource::ProgramBase& program, const std::pmr::string& samplerName, RenderTextureSlotID rendererTextureSlot)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::BindTextureSamplerToRendererTextureSlot(*(Resource::Program_OpenGL*) & program, samplerName, rendererTextureSlot);
		}

		template<GAPIType::ValueType GAPI>
		void UploadUniform(Resource::ProgramBase& program, size_t uniformIndex, const void* data)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::UploadUniform(*(Resource::ProgramBase*) & program, uniformIndex, data);
		}

		template<GAPIType::ValueType GAPI>
		void UploadUniform(Resource::ProgramBase& program, const std::pmr::string& uniformName, const void* data)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::UploadUniform(*(Resource::Program_OpenGL*) & program, uniformName, data);
		}

		// framebuffer

		template<GAPIType::ValueType GAPI>
		void ClearAttachment(Resource::FramebufferBase& framebuffer, uint32_t attachmentIndex, uint32_t value)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::UploadUniform(* (Resource::Framebuffer_OpenGL*) & framebuffer, attachmentIndex, value);
		}

		template<GAPIType::ValueType GAPI>
		void ClearAttachment(Resource::FramebufferBase& framebuffer, uint32_t attachmentIndex, float value)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::UploadUniform(*(Resource::Framebuffer_OpenGL*)&framebuffer, attachmentIndex, value);
		}

		template<GAPIType::ValueType GAPI>
		void ReadPixel(const Resource::FramebufferBase& framebuffer, uint32_t attachmentIndex, int x, int y, void* destination)
		{
			if constexpr (GAPI == GAPIType::OpenGL) OpenGL::UploadUniform(*(Resource::Framebuffer_OpenGL*) & framebuffer, attachmentIndex, x, y, destination);
		}
	}
}