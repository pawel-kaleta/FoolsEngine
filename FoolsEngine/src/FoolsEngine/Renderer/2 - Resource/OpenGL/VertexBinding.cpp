#include "FE_pch.h"

#include "Utils.h"

#include "FoolsEngine\Renderer\2 - Resource\VertexBinding.h"
#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include <glm\gtc\type_ptr.hpp>

namespace fe::Resource
{
	using namespace Description;

	void VertexBinding_OpenGL::Create()
	{
		FE_PROFILER_FUNC();

		const auto& layout = Library::Get().BufferLayouts[m_LayoutID];

		FE_CORE_ASSERT(layout.Type == Buffer::LayoutType::Vertex, "Vertex binding can only have vertex layout");

		glCreateVertexArrays(1, &m_VertexArrayID);
		glBindVertexArray(m_VertexArrayID);

		size_t rows;
		GLint columns;

		uint32_t buffer_element_index = 0;
		for (size_t i = 0; i < layout.Elements.size(); ++i)
		{
			const auto& element = layout.Elements[i];
			const auto& offset = layout.Offsets[i];

			Data::Primitive primitive = element.Primitive();
#pragma warning(disable : 4312)
#pragma warning(disable : 4267)
			switch (primitive.Value)
			{
			case Data::Primitive::Bool:
			case Data::Primitive::Int:
			case Data::Primitive::UInt:
				glEnableVertexAttribArray(buffer_element_index);
				glVertexAttribIPointer(
					buffer_element_index,
					element.ComponentCount(),
					Utils::DataPrimitiveToGLBaseType(primitive),
					layout.Stride,
					(const void*)offset
				);
				buffer_element_index++;
				break;

			case Data::Primitive::Double:
				glEnableVertexAttribArray(buffer_element_index);
				glVertexAttribLPointer(
					buffer_element_index,
					element.ComponentCount(),
					Utils::DataPrimitiveToGLBaseType(primitive),
					layout.Stride,
					(const void*)offset
				);
				buffer_element_index++;
				break;

			case Data::Primitive::Float:
				switch (element.Structure().Value)
				{
				case Data::Structure::Scalar:
				case Data::Structure::Vector:
					glEnableVertexAttribArray(buffer_element_index);
					glVertexAttribPointer(
						buffer_element_index,
						element.ComponentCount(),
						Utils::DataPrimitiveToGLBaseType(primitive),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.Stride,
						(const void*)offset
					);
					buffer_element_index++;
					break;

				case Data::Structure::Matrix:
					rows = Data::RowsOfMatrix(element.Type);
					columns = Data::ColumnsOfMatrix(element.Type);

					FE_LOG_CORE_DEBUG("rows: {0}, columns: {1}", rows, columns);
					FE_LOG_CORE_WARN("Vertex buffer layout setting, matrix - is this working?");
					for (size_t j = 0; j < rows; j++)
					{
						glEnableVertexAttribArray(buffer_element_index);
						glVertexAttribPointer(
							buffer_element_index,
							columns,
							Utils::DataPrimitiveToGLBaseType(primitive),
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

	void VertexBinding_OpenGL::Delete()
	{
		FE_PROFILER_FUNC();

		glDeleteVertexArrays(1, &m_VertexArrayID);
	}
}