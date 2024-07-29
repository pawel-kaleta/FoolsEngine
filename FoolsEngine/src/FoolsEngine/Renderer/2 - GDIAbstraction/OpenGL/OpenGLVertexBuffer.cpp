#include "FE_pch.h"

#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"

#include <glad\glad.h>

namespace fe
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
		: m_Size(size)
	{
		FE_PROFILER_FUNC();

		glCreateVertexArrays(1, &m_VertexArrayID);

		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, m_Size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
		: m_Size(size)
	{
		FE_PROFILER_FUNC();

		glCreateVertexArrays(1, &m_VertexArrayID);

		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, m_Size, vertices, GL_STATIC_DRAW);
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
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	uint32_t OpenGLVertexBuffer::GetSize() const
	{
		return m_Size;
	}

	void OpenGLVertexBuffer::SetLayout(const VertexData::Layout& layout)
	{
		FE_PROFILER_FUNC();
		m_Layout = layout;
		m_LayoutSet = true;

		static uint8_t rows;
		static uint8_t columns;

		glBindVertexArray(m_VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);

		uint32_t bufferElementIndex = 0;
		for (const auto& element : layout)
		{
			ShaderData::Primitive primitive = element.Primitive;
#pragma warning(disable : 4312)
			switch (primitive)
			{
			case ShaderData::Primitive::Bool:
			case ShaderData::Primitive::Int:
			case ShaderData::Primitive::UInt:
				glEnableVertexAttribArray(bufferElementIndex);
				glVertexAttribIPointer(
					bufferElementIndex,
					element.ComponentCount,
					SDPrimitiveToGLBaseType(primitive),
					layout.GetStride(),
					(const void*)element.Offset
				);
				bufferElementIndex++;
				break;

			case ShaderData::Primitive::Double:
				glEnableVertexAttribArray(bufferElementIndex);
				glVertexAttribLPointer(
					bufferElementIndex,
					element.ComponentCount,
					SDPrimitiveToGLBaseType(primitive),
					layout.GetStride(),
					(const void*)element.Offset
				);
				bufferElementIndex++;
				break;

			case ShaderData::Primitive::Float:
				switch (element.Structure)
				{
				case ShaderData::Structure::Scalar:
				case ShaderData::Structure::Vector:
					glEnableVertexAttribArray(bufferElementIndex);
					glVertexAttribPointer(
						bufferElementIndex,
						element.ComponentCount,
						SDPrimitiveToGLBaseType(primitive),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)element.Offset
					);
					bufferElementIndex++;
					break;

				case ShaderData::Structure::Matrix:
					rows = ShaderData::RowsOfMatrix(element.Type);
					columns = ShaderData::ColumnsOfMatrix(element.Type);

					FE_LOG_CORE_DEBUG("rows: {0}, columns: {1}", rows, columns);
					for (uint8_t i = 0; i < rows; i++)
					{
						glEnableVertexAttribArray(bufferElementIndex);
						glVertexAttribPointer(
							bufferElementIndex,
							columns,
							SDPrimitiveToGLBaseType(primitive),
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void*)element.Offset
						);
						glVertexAttribDivisor(bufferElementIndex, 1);
						bufferElementIndex++;
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

	const VertexData::Layout& OpenGLVertexBuffer::GetLayout() const
	{
		FE_PROFILER_FUNC();
		FE_CORE_ASSERT(m_LayoutSet, "Vertex Buffer has no layout!");

		return m_Layout;
	}

	void OpenGLVertexBuffer::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		FE_PROFILER_FUNC();

		glBindVertexArray(m_VertexArrayID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}