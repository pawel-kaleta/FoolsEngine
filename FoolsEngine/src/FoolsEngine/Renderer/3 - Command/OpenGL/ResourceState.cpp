#include "FE_pch.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/2 - Resource/OpenGL/Utils.h"
#include "FoolsEngine/Renderer/3 - Command/ResourceState.h"

namespace fe::Command
{
	namespace ResourceState::OpenGL
	{
		// shader interface setup

		void BindTextureSamplerToRendererTextureSlot(Resource::Program_OpenGL& program, size_t samplerIndex, RenderTextureSlotID rendererTextureSlot)
		{
			GLuint texture_unit = rendererTextureSlot;
			glUniform1ui(program.BindingLocations.TextureSamplers[samplerIndex], texture_unit);
		}

		void BindTextureSamplerToRendererTextureSlot(Resource::Program_OpenGL& program, const std::pmr::string& samplerName, RenderTextureSlotID rendererTextureSlot)
		{
			const auto& lib = Description::Library::Get();
			const auto& spec = lib.ProgramSpecs[program.SpecificationID];

			for (size_t i = 0; i < spec.TextureSamplerIDs.Count; ++i)
			{
				const auto& sampler = lib.TextureSamplers[spec.TextureSamplerIDs[i]];

				if (sampler.Name.compare(samplerName))
				{
					BindTextureSamplerToRendererTextureSlot(program, i, rendererTextureSlot);
					return;
				}
			}
		}

		void UploadUniform(Description::Data::Type type, uint32_t count, GLint location, const void* data)
		{
			if (!count)
			{
				FE_CORE_ASSERT(false, "Count must be positive");
				return;
			}

			switch (type)
			{
			case Description::Data::Type::None:
				FE_CORE_ASSERT(false, "Unknown Shader Data Type of uniform!");
				return;
			case Description::Data::Type::Bool:
			case Description::Data::Type::Int:
				glUniform1iv(location, count, (GLint*)data);
				return;
			case Description::Data::Type::Bool2:
			case Description::Data::Type::Int2:
				glUniform2iv(location, count, (GLint*)data);
				return;
			case Description::Data::Type::Bool3:
			case Description::Data::Type::Int3:
				glUniform3iv(location, count, (GLint*)data);
				return;
			case Description::Data::Type::Bool4:
			case Description::Data::Type::Int4:
				glUniform4iv(location, count, (GLint*)data);
				return;
			case Description::Data::Type::UInt:
				glUniform1uiv(location, count, (GLuint*)data);
				return;
			case Description::Data::Type::UInt2:
				glUniform2uiv(location, count, (GLuint*)data);
				return;
			case Description::Data::Type::UInt3:
				glUniform3uiv(location, count, (GLuint*)data);
				return;
			case Description::Data::Type::UInt4:
				glUniform4uiv(location, count, (GLuint*)data);
				return;
			case Description::Data::Type::Float:
				glUniform1fv(location, count, (GLfloat*)data);
				return;
			case Description::Data::Type::Float2:
				glUniform2fv(location, count, (GLfloat*)data);
				return;
			case Description::Data::Type::Float3:
				glUniform3fv(location, count, (GLfloat*)data);
				return;
			case Description::Data::Type::Float4:
				glUniform4fv(location, count, (GLfloat*)data);
				return;
			case Description::Data::Type::Double:
			case Description::Data::Type::Double2:
			case Description::Data::Type::Double3:
			case Description::Data::Type::Double4:
				FE_CORE_ASSERT(false, "Double precision Shader Data Type of uniform not supported!");
				return;
			case Description::Data::Type::Mat2:
				glUniformMatrix2fv(location, count, false, (GLfloat*)data);
				return;
			case Description::Data::Type::Mat2x3:
				glUniformMatrix2x3fv(location, count, false, (GLfloat*)data);
				return;
			case Description::Data::Type::Mat2x4:
				glUniformMatrix2x4fv(location, count, false, (GLfloat*)data);
				return;
			case Description::Data::Type::Mat3x2:
				glUniformMatrix3x2fv(location, count, false, (GLfloat*)data);
				return;
			case Description::Data::Type::Mat3:
				glUniformMatrix3fv(location, count, false, (GLfloat*)data);
				return;
			case Description::Data::Type::Mat3x4:
				glUniformMatrix3x4fv(location, count, false, (GLfloat*)data);
				return;
			case Description::Data::Type::Mat4x2:
				glUniformMatrix4x2fv(location, count, false, (GLfloat*)data);
				return;
			case Description::Data::Type::Mat4x3:
				glUniformMatrix4x3fv(location, count, false, (GLfloat*)data);
				return;
			case Description::Data::Type::Mat4:
				glUniformMatrix4fv(location, count, false, (GLfloat*)data);
				return;
			default:
				FE_CORE_ASSERT(false, "Unrecognised Shader Data Type of uniform!");
				return;
			}
		}

		void UploadUniform(Resource::Program_OpenGL& program, size_t uniformIndex, const void* data)
		{
			const auto& spec = Description::Library::Get().ProgramSpecs[program.SpecificationID];
			const auto& uniforms = Description::Library::Get().BufferLayouts[spec.MainUniformsLayoutID];
			const auto& uniform = uniforms.Elements[uniformIndex];

			Description::Data::Type type = uniform.Type;
			uint32_t count = uniform.Count;
			GLint location = program.BindingLocations.MainUniforms[uniformIndex];

			UploadUniform(type, count, location, data);
		}

		void UploadUniform(Resource::Program_OpenGL& program, const std::pmr::string& uniformName, const void* data)
		{
			const auto& spec = Description::Library::Get().ProgramSpecs[program.SpecificationID];

			const auto& uniforms = Description::Library::Get().BufferLayouts[spec.MainUniformsLayoutID];

			for (size_t i = 0; i < uniforms.Elements.size(); ++i)
			{
				const auto& uniform = uniforms.Elements[i];

				if (uniform.Name.compare(uniformName))
				{
					Description::Data::Type type = uniform.Type;
					uint32_t count = uniform.Count;
					GLint location = program.BindingLocations.MainUniforms[i];

					UploadUniform(type, count, location, data);

					return;
				}
			}
		}

		// framebuffer

		void ClearAttachment(Resource::Framebuffer_OpenGL& framebuffer, uint32_t attachmentIndex, uint32_t value)
		{
			FE_PROFILER_FUNC();

			FE_CORE_ASSERT(attachmentIndex < framebuffer.ColorAttachmentOpenGLIDs.size(), "Framebuffer attachment index out of bounds");

			const auto& spec = Description::Library::Get().FramebufferSpecs[framebuffer.SpecificationID];

			auto& format = spec.ColorAttachments[attachmentIndex].Format;
			glClearTexImage(framebuffer.ColorAttachmentOpenGLIDs[attachmentIndex], 0, Resource::Utils::FormatToGLFormat(format), GL_UNSIGNED_INT, &value);
		}

		void ClearAttachment(Resource::Framebuffer_OpenGL& framebuffer, uint32_t attachmentIndex, float value)
		{
			FE_PROFILER_FUNC();

			FE_CORE_ASSERT(attachmentIndex < framebuffer.ColorAttachmentOpenGLIDs.size(), "Framebuffer attachment index out of bounds");

			const auto& spec = Description::Library::Get().FramebufferSpecs[framebuffer.SpecificationID];

			auto& format = spec.ColorAttachments[attachmentIndex].Format;
			glClearTexImage(framebuffer.ColorAttachmentOpenGLIDs[attachmentIndex], 0, Resource::Utils::FormatToGLFormat(format), GL_FLOAT, &value);
		}

		void ReadPixel(const Resource::Framebuffer_OpenGL& framebuffer, uint32_t attachmentIndex, int x, int y, void* destination)
		{
			FE_PROFILER_FUNC();

			FE_CORE_ASSERT(attachmentIndex < framebuffer.ColorAttachmentOpenGLIDs.size(), "Framebuffer attachment index out of bounds");

			glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

			const auto& spec = Description::Library::Get().FramebufferSpecs[framebuffer.SpecificationID];
			auto& format = spec.ColorAttachments[attachmentIndex].Format;
			GLenum glFormat = Resource::Utils::FormatToGLFormat(format);
			GLenum glType = Resource::Utils::FormatToGLType(format);
			glReadPixels(x, y, 1, 1, glFormat, glType, destination);
		}

		void Clear()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}


	}
}