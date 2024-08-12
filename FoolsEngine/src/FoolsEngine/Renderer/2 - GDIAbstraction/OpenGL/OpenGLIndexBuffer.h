#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\IndexBuffer.h"

namespace fe
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

	private:
		uint32_t m_ID;
	};
}