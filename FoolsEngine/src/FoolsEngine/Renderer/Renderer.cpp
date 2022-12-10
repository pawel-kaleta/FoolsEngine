#include "FE_pch.h"
#include "Renderer.h"

namespace fe
{
	void Renderer::Init()
	{
		
	}

	void Renderer::BeginScene()
	{

	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		// TO DO: adding into a queue for future collective draw at the end of main loop
		
		vertexArray->Bind();
		RenderCommands::DrawIndexed(vertexArray);
	}
}