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

		shader->UploadUniform(
			Uniform("u_ViewProjection", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(s_SceneData->VPMatrix)
		);
		shader->UploadUniform(
			Uniform("u_Transform", ShaderData::Type::Mat4),
			(void*)glm::value_ptr(transform)
		);

		for (auto& uniform : materialInstance->GetMaterial()->GetUniforms())
		{
			void* dataPointer = materialInstance->GetUniformValuePtr(uniform);
			shader->UploadUniform(uniform, dataPointer);
		}

		vertexArray->Bind();
		RenderCommands::DrawIndexed(vertexArray);
	}
}