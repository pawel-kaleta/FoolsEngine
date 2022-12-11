#include "FE_pch.h"
#include "Renderer.h"

namespace fe
{
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		
	}

	void Renderer::BeginScene(OrtographicCamera& camera)
	{
		m_SceneData->VPMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
	{
		// TO DO: adding into a queue for future collective draw at the end of main loop
		
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProjection", m_SceneData->VPMatrix);

		vertexArray->Bind();
		RenderCommands::DrawIndexed(vertexArray);
	}
}