#pragma once
#include "FE_pch.h"
#include "FoolsEngine\Renderer\1 - Primitives\BufferData.h"

namespace fe
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		virtual uint32_t GetSize() const  = 0;

		virtual void SetLayout(const BufferData::Layout& layout) = 0;
		virtual const BufferData::Layout& GetLayout() const = 0;

		static Scope<VertexBuffer> Create(uint32_t size);
		static Scope<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Scope<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}