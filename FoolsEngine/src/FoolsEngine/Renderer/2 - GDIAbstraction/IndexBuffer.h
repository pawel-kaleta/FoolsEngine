#pragma once
#include "FE_pch.h"
#include "FoolsEngine\Renderer\1 - Primitives\VertexData.h"

namespace fe
{
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		uint32_t GetCount() const { return m_Count; }

		static Scope<IndexBuffer> Create(uint32_t* indices, uint32_t count);

	protected:
		IndexBuffer() = default;
		IndexBuffer(uint32_t count) : m_Count(count) { }
		uint32_t m_Count = 0;
	};
}