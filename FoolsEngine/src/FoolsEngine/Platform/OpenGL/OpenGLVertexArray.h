#pragma once
#include "FoolsEngine\Renderer\APIAbstraction\VertexArray.h"

namespace fe
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;

		virtual const Ref<IndexBuffer> GetIndexBuffer() const override { return m_IndexBuffer; }
		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }

	private:
		Ref<IndexBuffer> m_IndexBuffer;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		uint32_t m_VertexBufferIndex = 0;
		uint32_t m_ID = 0;
	};

}
