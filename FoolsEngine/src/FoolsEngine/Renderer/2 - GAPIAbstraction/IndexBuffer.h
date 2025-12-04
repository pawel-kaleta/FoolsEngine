#pragma once

namespace fe
{
	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		IndexBuffer(uint32_t count) : Count(count) { }

		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Scope<IndexBuffer> Create(uint32_t* indices, uint32_t count);

		uint32_t Count = 0;
	};
}