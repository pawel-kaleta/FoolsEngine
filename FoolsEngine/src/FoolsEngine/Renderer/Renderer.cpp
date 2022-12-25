#include "FE_pch.h"
#include "Renderer.h"

namespace fe
{
	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

	void Renderer::Init()
	{
		FE_PROFILER_FUNC();
	}

	void Renderer::BeginScene(OrtographicCamera& camera)
	{
		FE_PROFILER_FUNC();
		s_SceneData->VPMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
		FE_PROFILER_FUNC();
	}

	void Renderer::Submit(
		const Ref<VertexArray>& vertexArray,
		const Ref<MaterialInstance>& materialInstance,
		const glm::mat4& transform)
	{
		FE_PROFILER_FUNC();

		// TO DO: adding into a queue for future collective draw at the end of main loop
		const Ref<Shader>& shader = materialInstance->GetMaterial()->GetShader();

		shader->Bind();
		shader->UploadUniform("u_ViewProjection", (void*)glm::value_ptr(s_SceneData->VPMatrix), SDType::Mat4);
		shader->UploadUniform("u_Transform",      (void*)glm::value_ptr(transform),             SDType::Mat4);

		for (auto uniform : materialInstance->GetMaterial()->GetUniforms())
		{
			const std::string& name = uniform.GetName();
			void* dataPointer = materialInstance->GetUniformValuePtr(name);
			SDType type = uniform.GetType();
			shader->UploadUniform(name, dataPointer, type);
		}

		vertexArray->Bind();
		RenderCommands::DrawIndexed(vertexArray);
	}
}