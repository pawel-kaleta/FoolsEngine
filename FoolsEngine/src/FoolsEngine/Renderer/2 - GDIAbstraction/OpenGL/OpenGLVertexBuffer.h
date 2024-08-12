#pragma once
#include "FoolsEngine\Renderer\1 - Primitives\VertexData.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\VertexBuffer.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\IndexBuffer.h"

namespace fe
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SendDataToGPU(const void* data, uint32_t size) override;

		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;
		virtual const Ref<IndexBuffer> GetIndexBuffer() const override { return m_IndexBuffer; }

		virtual uint32_t GetSize() const override;

		virtual void SetLayout(const VertexData::Layout& layout) override;
		virtual const VertexData::Layout& GetLayout() const override;

	private:
		uint32_t m_ID;
		uint32_t m_Size;
		Ref<IndexBuffer> m_IndexBuffer;
		uint32_t m_VertexArrayID;
		VertexData::Layout m_Layout;
		bool m_LayoutSet = false;
	};
}
