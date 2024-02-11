#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\IndexBuffer.h"

#include <glad\glad.h>

namespace fe
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override;

	private:
		uint32_t m_ID;
		uint32_t m_Count;
	};

	static GLenum SDPrimitiveToGLBaseType(ShaderData::Primitive primitive)
	{
		const static GLenum LookupTable[] = { GL_BOOL, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_DOUBLE };
		return LookupTable[(int)primitive - 1];
	};
}