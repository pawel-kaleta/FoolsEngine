#pragma once

#include "FE_pch.h"
#include "Buffers.h"

// TO DO
// Distroy this abomination, as only OpenGL has a concept of VertexArray.
// at GDI agnostic level this should not exist.

namespace fe
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;

		virtual const Ref<IndexBuffer> GetIndexBuffer() const = 0;
		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;

		static Scope<VertexArray> Create();
	};
}