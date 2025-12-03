#include "FE_pch.h"
#include "OpenGLVertexBuffer.h"

#include "FoolsEngine\Renderer\1 - Description\Data.h"

#include <glad\glad.h>

namespace fe
{
	GLenum SDPrimitiveToGLBaseType(Description::Data::Primitive primitive)
	{
		const static GLenum s_lookup_table[] = { GL_BOOL, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_DOUBLE };
		return s_lookup_table[primitive.ToInt() - 1];
	};

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
		: m_Size(size)
	{
		FE_PROFILER_FUNC();

		auto logging_level = Log::GetCoreLoggingLevel();
		Log::SetCoreLoggingLevel(Log::LoggingLevel::Info);

		glCreateVertexArrays(1, &m_VertexArrayID);

		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, m_Size, nullptr, GL_DYNAMIC_DRAW);

		Log::SetCoreLoggingLevel(logging_level);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
		: m_Size(size)
	{
		FE_PROFILER_FUNC();

		auto logging_level = Log::GetCoreLoggingLevel();
		Log::SetCoreLoggingLevel(Log::LoggingLevel::Info);

		glCreateVertexArrays(1, &m_VertexArrayID);

		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, m_Size, vertices, GL_STATIC_DRAW);

		Log::SetCoreLoggingLevel(logging_level);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		FE_PROFILER_FUNC();
		glDeleteBuffers(1, &m_ID);
		glDeleteVertexArrays(1, &m_VertexArrayID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		FE_PROFILER_FUNC();

		glBindVertexArray(m_VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		m_IndexBuffer->Bind();
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		FE_PROFILER_FUNC();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_IndexBuffer->Unbind();
		glBindVertexArray(0);
	}

	void OpenGLVertexBuffer::SendDataToGPU(const void* data, uint32_t size)
	{
		FE_CORE_ASSERT(size <= m_Size, "Too much data for this VertexBuffer!");

		auto logging_level = Log::GetCoreLoggingLevel();
		Log::SetCoreLoggingLevel(Log::LoggingLevel::Info);

		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

		Log::SetCoreLoggingLevel(logging_level);
	}

	uint32_t OpenGLVertexBuffer::GetSize() const
	{
		return m_Size;
	}

	void OpenGLVertexBuffer::SetLayout(const Description::Buffer::Layout& layout)
	{
		using namespace Description;

		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(layout.Type == Buffer::LayoutType::Vertex, "Vertex buffer can only have vertex layout");

		m_Layout = &layout;
		m_LayoutSet = true;

		glBindVertexArray(m_VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);

		size_t rows;
		GLint columns;

		uint32_t buffer_element_index = 0;
		for (size_t i = 0; i < layout.Elements.size(); ++i)
		{
			const auto& element = layout.Elements[i];
			const auto& offset = layout.Offsets[i];

			Data::Primitive primitive = element.Primitive();
#pragma warning(disable : 4312)
			switch (primitive.Value)
			{
			case Data::Primitive::Bool:
			case Data::Primitive::Int:
			case Data::Primitive::UInt:
				glEnableVertexAttribArray(buffer_element_index);
				
				glVertexAttribIPointer(
					buffer_element_index,
					element.ComponentCount(),
					SDPrimitiveToGLBaseType(primitive),
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
					SDPrimitiveToGLBaseType(primitive),
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
						SDPrimitiveToGLBaseType(primitive),
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
							SDPrimitiveToGLBaseType(primitive),
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.Stride,
							(const void*)offset
						);
						glVertexAttribDivisor(buffer_element_index, 1);
						buffer_element_index++;
					}
					break;

				default:
					FE_CORE_ASSERT(false, "Unknown Shader Data Structure!");
				}
				break;

			default:
				FE_CORE_ASSERT(false, "Unknown Shader Data Primitive!");
			}
#pragma warning(default : 4312)
		}
	}

	const Description::Buffer::Layout& OpenGLVertexBuffer::GetLayout() const
	{
		FE_PROFILER_FUNC();
		FE_CORE_ASSERT(m_LayoutSet, "Vertex Buffer has no layout!");

		return *m_Layout;
	}

	void OpenGLVertexBuffer::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		FE_PROFILER_FUNC();

		glBindVertexArray(m_VertexArrayID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}