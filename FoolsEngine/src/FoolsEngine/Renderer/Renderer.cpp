#include "FE_pch.h"
#include "Renderer.h"

namespace fe
{
	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		
	}

	void Renderer::BeginScene(OrtographicCamera& camera)
	{
		s_SceneData->VPMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(
		const std::shared_ptr<VertexArray>& vertexArray,
		const std::shared_ptr<Shader>& shader,
		const glm::mat4& transform)
	{
		// TO DO: adding into a queue for future collective draw at the end of main loop
		
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProjection", s_SceneData->VPMatrix);
		shader->UploadUniformMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommands::DrawIndexed(vertexArray);
	}
}