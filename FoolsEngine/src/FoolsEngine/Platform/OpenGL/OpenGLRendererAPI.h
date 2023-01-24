#pragma once

#include "FoolsEngine\Renderer\APIAbstraction\RendererAPI.h"

namespace fe
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		OpenGLRendererAPI() { FE_LOG_CORE_DEBUG("OpenGLRendererAPI creation!"); };
		virtual void Init() override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	
	private:

	};
}

