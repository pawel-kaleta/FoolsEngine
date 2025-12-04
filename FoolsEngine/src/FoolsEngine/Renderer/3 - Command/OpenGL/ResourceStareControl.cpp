#include "FE_pch.h"


#include "FoolsEngine\Renderer\2 - Resource\OpenGL\Utils.h"
#include "FoolsEngine\Renderer\3 - Command\ResourceStateControl.h"

namespace fe::Command
{
	namespace ResourceStateControl::OpenGL
	{
		void BindToRendererTextureSlot(uint32_t rendererTextureSlot, const Resource::Texture_OpenGL& texture)
		{
			glBindTextureUnit(rendererTextureSlot, texture.TextureOpenGLID);
		}

		void BindVertexBuffer(const Resource::VertexBuffer_OpenGL& vertexBuffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.VertexBufferID);
		}

		void BindVertexBinding(const Resource::VertexBinding_OpenGL& vertexBinding)
		{
			glBindVertexArray(vertexBinding.VertexArrayID);
		}

		void BindIndexBuffer(const Resource::IndexBuffer_OpenGL& indexBuffer)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.IndexBufferOpenGLID);
		}

		void ReserveOrUploadVertexBuffer(Resource::VertexBuffer_OpenGL& vertexBuffer, size_t size, const void * data)
		{
			vertexBuffer.Size = size;
			switch (vertexBuffer.UploadType)
			{
			case Description::Buffer::UploadType::Static:	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); break;
			case Description::Buffer::UploadType::Dynamic:	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW); break;
			case Description::Buffer::UploadType::Stream:	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STREAM_DRAW); break;
			default:
				FE_LOG_CORE_ERROR("Unrecognized VertexBuffer UploadType, defaulting to GL_DYNAMIC_DRAW");
				glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
			}
		}

		void SetLayoutInVertexBinding(Resource::VertexBinding_OpenGL& vertexBinding)
		{
			FE_PROFILER_FUNC();

			const auto& layout = Description::Library::Get().BufferLayouts[vertexBinding.LayoutID];

			FE_CORE_ASSERT(layout.Type == Description::Buffer::LayoutType::Vertex, "Vertex binding can only have vertex layout");

			size_t rows;
			GLint columns;

			uint32_t buffer_element_index = 0;
			for (size_t i = 0; i < layout.Elements.size(); ++i)
			{
				const auto& element = layout.Elements[i];
				const auto& offset = layout.Offsets[i];

				Description::Data::Primitive primitive = element.Primitive();
#pragma warning(disable : 4312)
#pragma warning(disable : 4267)
				switch (primitive.Value)
				{
				case Description::Data::Primitive::Bool:
				case Description::Data::Primitive::Int:
				case Description::Data::Primitive::UInt:
					glEnableVertexAttribArray(buffer_element_index);
					glVertexAttribIPointer(
						buffer_element_index,
						element.ComponentCount(),
						Resource::Utils::DataPrimitiveToGLBaseType(primitive),
						layout.Stride,
						(const void*)offset
					);
					buffer_element_index++;
					break;

				case Description::Data::Primitive::Double:
					glEnableVertexAttribArray(buffer_element_index);
					glVertexAttribLPointer(
						buffer_element_index,
						element.ComponentCount(),
						Resource::Utils::DataPrimitiveToGLBaseType(primitive),
						layout.Stride,
						(const void*)offset
					);
					buffer_element_index++;
					break;

				case Description::Data::Primitive::Float:
					switch (element.Structure().Value)
					{
					case Description::Data::Structure::Scalar:
					case Description::Data::Structure::Vector:
						glEnableVertexAttribArray(buffer_element_index);
						glVertexAttribPointer(
							buffer_element_index,
							element.ComponentCount(),
							Resource::Utils::DataPrimitiveToGLBaseType(primitive),
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.Stride,
							(const void*)offset
						);
						buffer_element_index++;
						break;

					case Description::Data::Structure::Matrix:
						rows = Description::Data::RowsOfMatrix(element.Type);
						columns = Description::Data::ColumnsOfMatrix(element.Type);

						FE_LOG_CORE_DEBUG("rows: {0}, columns: {1}", rows, columns);
						FE_LOG_CORE_WARN("Vertex buffer layout setting, matrix - is this working?");
						for (size_t j = 0; j < rows; j++)
						{
							glEnableVertexAttribArray(buffer_element_index);
							glVertexAttribPointer(
								buffer_element_index,
								columns,
								Resource::Utils::DataPrimitiveToGLBaseType(primitive),
								element.Normalized ? GL_TRUE : GL_FALSE,
								layout.Stride,
								(const void*)offset
							);
							glVertexAttribDivisor(buffer_element_index, 1);
							buffer_element_index++;
						}
						break;

					default:
						FE_CORE_ASSERT(false, "Unknown Description::Data::Structure!");
					}
					break;

				default:
					FE_CORE_ASSERT(false, "Unknown Description::Data::Primitive!");
				}
#pragma warning(default : 4312)
#pragma warning(default : 4267)
			}
		}

		void ReserveOrUploadTexture(Resource::Texture_OpenGL& texture, const void* data)
		{
			FE_PROFILER_FUNC();

			const auto& spec = Description::Library::Get().TextureSpecs[texture.SpecificationID];

			auto format = Resource::Utils::FormatToGLFormat(spec.Format);
			auto internal_format = Resource::Utils::FormatToGLInternalFormat(spec.Format);

			glTexImage2D(GL_TEXTURE_2D, 0, internal_format, texture.Width, texture.Height, 0, format, GL_UNSIGNED_BYTE, data);

			if (spec.Mipmapping == Description::Texture::Mipmapping::Nearest || spec.Mipmapping == Description::Texture::Mipmapping::Liniear)
				glGenerateMipmap(GL_TEXTURE_2D);
		}

		void BindTextureSamplerToRendererTextureSlot(const Resource::Program_OpenGL& program, size_t samplerIndex, RenderTextureSlotID rendererTextureSlot)
		{
			GLuint texture_unit = rendererTextureSlot;
			glUniform1ui(program.BindingLocations.TextureSamplers[samplerIndex], texture_unit);
		}

		void BindTextureSamplerToRendererTextureSlot(const Resource::Program_OpenGL& program, const std::pmr::string& samplerName, RenderTextureSlotID rendererTextureSlot)
		{
			const auto& spec = Description::Library::Get().ProgramSpecs[program.SpecificationID];

			for (size_t i = 0; i < spec.TextureSamplers.size(); ++i)
			{
				const auto& sampler = spec.TextureSamplers[i];

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

		void UploadUniform(const Resource::Program_OpenGL& program, size_t uniformIndex, const void* data)
		{
			const auto& spec = Description::Library::Get().ProgramSpecs[program.SpecificationID];
			const auto& uniforms = Description::Library::Get().BufferLayouts[spec.MainUniformsLayoutID];
			const auto& uniform = uniforms.Elements[uniformIndex];

			Description::Data::Type type = uniform.Type;
			uint32_t count = uniform.Count;
			GLint location = program.BindingLocations.MainUniforms[uniformIndex];

			UploadUniform(type, count, location, data);
		}

		void UploadUniform(const Resource::Program_OpenGL& program, const std::pmr::string& uniformName, const void* data)
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

		void ReserveOrUploadIndexBuffer(Resource::IndexBuffer_OpenGL& indexBuffer, uint32_t count, const uint32_t* indices)
		{

			switch (indexBuffer.UploadType)
			{
			case Description::Buffer::UploadType::Static:	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW); break;
			case Description::Buffer::UploadType::Dynamic:	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_DYNAMIC_DRAW); break;
			case Description::Buffer::UploadType::Stream:	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STREAM_DRAW); break;
			default:
				FE_LOG_CORE_ERROR("Unrecognized IndexBuffer UploadType, defaulting to GL_DYNAMIC_DRAW");
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_DYNAMIC_DRAW);
			}
		}


	}
}