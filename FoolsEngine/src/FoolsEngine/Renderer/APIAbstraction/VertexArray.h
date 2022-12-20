#pragma once

#include "FE_pch.h"
#include "Buffers.h"

// TO DO
// Distroy this abomination, as only OpenGL has a concept of VertexArray.
// On API abstraction level this should not exist.

namespace fe
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;
		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;

		virtual const std::shared_ptr<IndexBuffer> GetIndexBuffer() const = 0;
		virtual const std::vector<std::shared_ptr<VertexBuffer>> GetVertexBuffers() const = 0;

		static VertexArray* Create();
	};
}