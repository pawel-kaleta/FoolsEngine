#include "FE_pch.h"

#include "Utils.h"

#include "FoolsEngine\Renderer\2 - Resource\VertexArray.h"
#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include <glm\gtc\type_ptr.hpp>

namespace fe::Resource
{
	using namespace Description;

	void VertexArray_OpenGL::Create()
	{
		FE_PROFILER_FUNC();

		glCreateVertexArrays(1, &OpenGLID);
	}

	void VertexArray_OpenGL::Delete()
	{
		FE_PROFILER_FUNC();

		glDeleteVertexArrays(1, &OpenGLID);
	}

	void VertexArray_OpenGL::BindVertexBuffer(const StaticBufferBase& buffer, uint32_t offset)
	{
		FE_PROFILER_FUNC();

		VertexOffset = offset;

		const auto& layout = Description::Library::Get().BufferLayouts[LayoutID];

		FE_CORE_ASSERT(layout.Type == Description::Buffer::LayoutType::Vertex, "Vertex Array can only have vertex layout");

		size_t rows;
		GLint columns;
		
		StaticBuffer_OpenGL & vertex_buffer = * (StaticBuffer_OpenGL*) & buffer;

		uint32_t buffer_element_index = 0;
		for (size_t i = 0; i < layout.Elements.size(); ++i)
		{
			const auto& element = layout.Elements[i];
			const auto& element_offset = layout.Offsets[i];

			Description::Data::Primitive primitive = element.Primitive();
			GLenum type = Resource::Utils::DataPrimitiveToGLBaseType(primitive);
			GLboolean normalized = element.Normalized ? GL_TRUE : GL_FALSE;

#pragma warning(disable : 4312)
#pragma warning(disable : 4267)
			switch (primitive.Value)
			{
			case Description::Data::Primitive::Bool:
			case Description::Data::Primitive::Int:
			case Description::Data::Primitive::UInt:
				glEnableVertexArrayAttrib(OpenGLID, buffer_element_index);
				glVertexArrayVertexBuffer(OpenGLID, buffer_element_index, vertex_buffer.OpenGLID, layout.Stride, offset);
				glVertexArrayAttribIFormat(OpenGLID, buffer_element_index, element.ComponentCount(), type, element_offset);
				buffer_element_index++;
				break;

			case Description::Data::Primitive::Double:
				glEnableVertexArrayAttrib(OpenGLID, buffer_element_index);
				glVertexArrayVertexBuffer(OpenGLID, buffer_element_index, vertex_buffer.OpenGLID, layout.Stride, offset);
				glVertexArrayAttribLFormat(OpenGLID, buffer_element_index, element.ComponentCount(), type, element_offset);
				buffer_element_index++;
				break;

			case Description::Data::Primitive::Float:
				switch (element.Structure().Value)
				{
				case Description::Data::Structure::Scalar:
				case Description::Data::Structure::Vector:
					glEnableVertexArrayAttrib(OpenGLID, buffer_element_index);
					glVertexArrayVertexBuffer(OpenGLID, buffer_element_index, vertex_buffer.OpenGLID, layout.Stride, offset);
					glVertexArrayAttribFormat(OpenGLID, buffer_element_index, element.ComponentCount(), type, normalized, element_offset);
					buffer_element_index++;
					break;

				case Description::Data::Structure::Matrix:
					rows = Description::Data::RowsOfMatrix(element.Type);
					columns = Description::Data::ColumnsOfMatrix(element.Type);

					FE_LOG_CORE_DEBUG("rows: {0}, columns: {1}", rows, columns);
					FE_LOG_CORE_WARN("Vertex buffer layout setting, matrix - is this working?");
					for (size_t j = 0; j < rows; j++)
					{
						glEnableVertexArrayAttrib(OpenGLID, buffer_element_index);
						glVertexArrayVertexBuffer(OpenGLID, buffer_element_index, vertex_buffer.OpenGLID, layout.Stride, offset);
						glVertexArrayAttribFormat(OpenGLID, buffer_element_index, element.ComponentCount(), type, normalized, element_offset);
						glVertexArrayBindingDivisor(OpenGLID, buffer_element_index, 1);
						glVertexArrayAttribBinding(OpenGLID, buffer_element_index, buffer_element_index);
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

	void VertexArray_OpenGL::BindIndexBuffer(const StaticBufferBase& buffer, uint32_t offset, uint32_t indexCount)
	{
		glVertexArrayElementBuffer(OpenGLID, (*(StaticBuffer_OpenGL*)&buffer).OpenGLID);
		IndexCount = indexCount;
		IndexOffset = offset;
	}
}