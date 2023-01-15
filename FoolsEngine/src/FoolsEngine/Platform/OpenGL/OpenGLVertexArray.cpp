#include "FE_pch.h"
#include "OpenGLVertexArray.h"
#include "OpenGLBuffers.h"

#include <glad\glad.h>

namespace fe
{
	fe::OpenGLVertexArray::OpenGLVertexArray()
	{
		FE_PROFILER_FUNC();

		glCreateVertexArrays(1, &m_ID);
	}

	fe::OpenGLVertexArray::~OpenGLVertexArray()
	{
		FE_PROFILER_FUNC();

		glDeleteVertexArrays(1, &m_ID);
	}

	void fe::OpenGLVertexArray::Bind() const
	{
		FE_PROFILER_FUNC();

		glBindVertexArray(m_ID);
	}

	void fe::OpenGLVertexArray::Unbind() const
	{
		FE_PROFILER_FUNC();

		glBindVertexArray(0);
	}

	void fe::OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		FE_PROFILER_FUNC();

		glBindVertexArray(m_ID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

	void fe::OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		FE_PROFILER_FUNC();

		static uint8_t rows;
		static uint8_t columns;

		glBindVertexArray(m_ID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			ShaderData::Primitive primitive = element.Primitive;
#pragma warning(disable : 4312)
			switch (primitive)
			{
			case ShaderData::Primitive::Bool:
			case ShaderData::Primitive::Int:
			case ShaderData::Primitive::UInt:
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribIPointer(
					m_VertexBufferIndex,
					element.ComponentCount,
					SDPrimitiveToGLBaseType(primitive),
					layout.GetStride(),
					(const void*)element.Offset
				);
				m_VertexBufferIndex++;
				break;

			case ShaderData::Primitive::Double:
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribLPointer(
					m_VertexBufferIndex,
					element.ComponentCount,
					SDPrimitiveToGLBaseType(primitive),
					layout.GetStride(),
					(const void*)element.Offset
				);
				m_VertexBufferIndex++;
				break;

			case ShaderData::Primitive::Float:
				switch (element.Structure)
				{
				case ShaderData::Structure::Scalar:
				case ShaderData::Structure::Vector:
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribPointer(
						m_VertexBufferIndex,
						element.ComponentCount,
						SDPrimitiveToGLBaseType(primitive),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)element.Offset
					);
					m_VertexBufferIndex++;
					break;

				case ShaderData::Structure::Matrix:
					rows = ShaderData::RowsOfMatrix(element.Type);
					columns = ShaderData::ColumnsOfMatrix(element.Type);

					FE_LOG_CORE_DEBUG("rows: {0}, columns: {1}", rows, columns);
					for (uint8_t i = 0; i < rows; i++)
					{
						glEnableVertexAttribArray(m_VertexBufferIndex);
						glVertexAttribPointer(
							m_VertexBufferIndex,
							columns,
							SDPrimitiveToGLBaseType(primitive),
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void*)element.Offset
						);
						glVertexAttribDivisor(m_VertexBufferIndex, 1);
						m_VertexBufferIndex++;
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
}