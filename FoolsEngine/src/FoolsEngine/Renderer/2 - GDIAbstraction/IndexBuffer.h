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

		virtual uint32_t GetCount() const = 0;

		static Scope<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}