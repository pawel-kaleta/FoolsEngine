#pragma once

#include "FE_pch.h"
#include "IndexBuffer.h"

namespace fe
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SendDataToGPU(const void* data, uint32_t size) = 0;

		virtual uint32_t GetSize() const = 0;

		virtual void SetLayout(const VertexData::Layout& layout) = 0;
		virtual const VertexData::Layout& GetLayout() const = 0;

		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;
		virtual const Ref<IndexBuffer> GetIndexBuffer() const = 0;

		static Scope<VertexBuffer> Create(uint32_t size);
		static Scope<VertexBuffer> Create(float* vertices, uint32_t size);
	};
}